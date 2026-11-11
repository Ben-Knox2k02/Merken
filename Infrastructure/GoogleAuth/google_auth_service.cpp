#include "google_auth_service.h"
#include "google_pkce.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

static const std::string kAuthUrl = "https://accounts.google.com/o/oauth2/v2/auth";
static const std::string kTokenUrl = "https://oauth2.googleapis.com/token";
static const std::string kCalendarScope = "https://www.googleapis.com/auth/calendar.events";
static const int kSignInTimeoutSeconds = 180;
static const long long kExpirySkewSeconds = 60; //refresh a minute early so a token never expires mid-request

static void LogAuthError(const std::string& operation, const std::string& detail){
	std::cerr << "[GoogleAuth] " << operation << " failed: " << detail << std::endl;
}

//percent-encodes a string for use in a URL or form body
static std::string UrlEncode(const std::string& value) {
	std::ostringstream escaped;
	escaped << std::hex << std::uppercase;
	for (unsigned char c : value){
		if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
		} else {
			escaped << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
		}
	}
	return escaped.str();
}

static int HexValue(char c){
	if (c >= '0' && c <= '9') { return c - '0'; }
	if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
	if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
	return -1;
}

static std::string UrlDecode(const std::string& value){
	std::string out;
	for (size_t i = 0; i < value.size(); ++i) {
		if (value[i] == '%' && i + 2 < value.size() && HexValue(value[i + 1]) >= 0 && HexValue(value[i + 2]) >= 0) {
			out.push_back(static_cast<char>(HexValue(value[i + 1]) * 16 + HexValue(value[i + 2])));
			i += 2;
		} else if (value[i] == '+') {
			out.push_back(' ');
		} else {
			out.push_back(value[i]);
		}
	}
	return out;
}

//builds key=value&key=value with every part percent-encoded
static std::string FormEncode(const std::vector<std::pair<std::string, std::string>>& fields){
	std::string out;
	for (const auto& field : fields) {
		if (!out.empty()) { out += "&"; }
		out += UrlEncode(field.first) + "=" + UrlEncode(field.second);
	}
	return out;
}

//reads one parameter from a request target like "/?code=abc&state=xyz"; returns "" if it is not there
static std::string GetQueryParam(const std::string& target, const std::string& name){
	const size_t questionMark = target.find('?');
	if (questionMark == std::string::npos) { return ""; }
	std::string query = target.substr(questionMark + 1);
	const size_t hash = query.find('#');
	if (hash != std::string::npos) { query = query.substr(0, hash); }

	size_t position = 0;
	while (position <= query.size()) {
		size_t ampersand = query.find('&', position);
		if (ampersand == std::string::npos) { ampersand = query.size(); }
		const std::string pair = query.substr(position, ampersand - position);
		const size_t equals = pair.find('=');
		const std::string key = equals == std::string::npos ? pair : pair.substr(0, equals);
		if (UrlDecode(key) == name) {
			return equals == std::string::npos ? "" : UrlDecode(pair.substr(equals + 1));
		}
		position = ampersand + 1;
	}
	return "";
}

//why a token request failed: the transport error, or Google's error and error_description
static std::string DescribeTokenFailure(const HttpResponse& response){
	if (!response.error.empty()) {
		return response.error;
	}
	nlohmann::json json = nlohmann::json::parse(response.body, nullptr, false);
	if (json.is_object() && json.contains("error") && json["error"].is_string()) {
		std::string text = json["error"].get<std::string>();
		if (json.contains("error_description") && json["error_description"].is_string()) {
			text += ": " + json["error_description"].get<std::string>();
		}
		return text;
	}
	return "HTTP " + std::to_string(response.statusCode);
}

//pulls the token fields out of Google's token response; false if there is no access token
static bool ParseTokenResponse(const std::string& body, std::string& accessToken, std::string& refreshToken, long long& expiresInSeconds){
	nlohmann::json json = nlohmann::json::parse(body, nullptr, false);
	if (!json.is_object()) { return false; }
	accessToken = (json.contains("access_token") && json["access_token"].is_string()) ? json["access_token"].get<std::string>() : "";
	refreshToken = (json.contains("refresh_token") && json["refresh_token"].is_string()) ? json["refresh_token"].get<std::string>() : "";
	expiresInSeconds = (json.contains("expires_in") && json["expires_in"].is_number()) ? json["expires_in"].get<long long>() : 3600;
	return !accessToken.empty();
}

bool GoogleAuthService::SignIn() {
	if (this->config.clientId.empty()) {
		LogAuthError("SignIn", "Google client ID is not configured (google_client.json)");
		return false;
	}

	const int port = this->loopbackReceiver.Start();
	if (port <= 0) {
		LogAuthError("SignIn", "Could not listen on a local port for Google's redirect");
		return false;
	}
	const std::string redirectUri = "http://127.0.0.1:" + std::to_string(port);

	//PKCE: the verifier stays here; only its hash goes to Google until the code exchange
	const std::string codeVerifier = RandomUrlSafeString(32);
	const std::string codeChallenge = CodeChallengeS256(codeVerifier);
	const std::string state = RandomUrlSafeString(16);

	const std::string url = kAuthUrl + "?" + FormEncode({
		{"client_id", this->config.clientId},
		{"redirect_uri", redirectUri},
		{"response_type", "code"},
		{"scope", kCalendarScope},
		{"code_challenge", codeChallenge},
		{"code_challenge_method", "S256"},
		{"state", state},
		{"access_type", "offline"},
		{"prompt", "consent"}
	});

	if (!this->browserLauncher.Open(url)) {
		this->loopbackReceiver.Stop();
		LogAuthError("SignIn", "Could not open the browser");
		return false;
	}

	const std::string target = this->loopbackReceiver.WaitForRedirect(kSignInTimeoutSeconds);
	this->loopbackReceiver.Stop();

	if (target.empty()) {
		LogAuthError("SignIn", "Timed out waiting for Google's redirect");
		return false;
	}
	const std::string error = GetQueryParam(target, "error");
	if (!error.empty()) {
		LogAuthError("SignIn", "Google returned an error: " + error);
		return false;
	}
	if (GetQueryParam(target, "state") != state) {
		LogAuthError("SignIn", "The state value did not match, so the response was ignored");
		return false;
	}
	const std::string code = GetQueryParam(target, "code");
	if (code.empty()) {
		LogAuthError("SignIn", "Google's redirect had no authorization code");
		return false;
	}

	return this->ExchangeCode(code, codeVerifier, redirectUri);
}

bool GoogleAuthService::ExchangeCode(const std::string& code, const std::string& codeVerifier, const std::string& redirectUri) {
	std::vector<std::pair<std::string, std::string>> fields = {
		{"grant_type", "authorization_code"},
		{"code", code},
		{"code_verifier", codeVerifier},
		{"redirect_uri", redirectUri},
		{"client_id", this->config.clientId}
	};
	if (!this->config.clientSecret.empty()) {
		fields.push_back({"client_secret", this->config.clientSecret});
	}

	HttpResponse response = this->httpClient.PostForm(kTokenUrl, FormEncode(fields));
	if (!response.Ok()) {
		LogAuthError("ExchangeCode", DescribeTokenFailure(response));
		return false;
	}

	std::string accessToken;
	std::string refreshToken;
	long long expiresIn = 0;
	if (!ParseTokenResponse(response.body, accessToken, refreshToken, expiresIn)) {
		LogAuthError("ExchangeCode", "Google's response had no access token");
		return false;
	}
	if (refreshToken.empty()) {
		LogAuthError("ExchangeCode", "Google's response had no refresh token");
		return false;
	}

	AppSettings settings = this->appSettingsService.GetSettings();
	settings.calendarApiKey = accessToken;
	settings.calendarRefreshToken = refreshToken;
	settings.calendarTokenExpiresAt = this->clock.NowSeconds() + expiresIn;
	if (!this->appSettingsService.SaveSettings(settings)) {
		LogAuthError("ExchangeCode", "Could not save the tokens");
		return false;
	}
	return true;
}

std::string GoogleAuthService::GetAccessToken() {
	AppSettings settings = this->appSettingsService.GetSettings();

	//no refresh token: use whatever access token was pasted into settings
	if (settings.calendarRefreshToken.empty()) {
		return settings.calendarApiKey;
	}

	const long long now = this->clock.NowSeconds();
	if (!settings.calendarApiKey.empty() && settings.calendarTokenExpiresAt > now + kExpirySkewSeconds) {
		return settings.calendarApiKey;
	}
	return this->RefreshAccessToken(settings);
}

std::string GoogleAuthService::RefreshAccessToken(AppSettings settings) {
	if (this->config.clientId.empty()) {
		LogAuthError("RefreshAccessToken", "Google client ID is not configured (google_client.json)");
		return "";
	}

	std::vector<std::pair<std::string, std::string>> fields = {
		{"grant_type", "refresh_token"},
		{"refresh_token", settings.calendarRefreshToken},
		{"client_id", this->config.clientId}
	};
	if (!this->config.clientSecret.empty()) {
		fields.push_back({"client_secret", this->config.clientSecret});
	}

	HttpResponse response = this->httpClient.PostForm(kTokenUrl, FormEncode(fields));
	if (!response.Ok()) {
		LogAuthError("RefreshAccessToken", DescribeTokenFailure(response));
		return "";
	}

	std::string accessToken;
	std::string newRefreshToken;
	long long expiresIn = 0;
	if (!ParseTokenResponse(response.body, accessToken, newRefreshToken, expiresIn)) {
		LogAuthError("RefreshAccessToken", "Google's response had no access token");
		return "";
	}

	settings.calendarApiKey = accessToken;
	settings.calendarTokenExpiresAt = this->clock.NowSeconds() + expiresIn;
	if (!newRefreshToken.empty()) {
		settings.calendarRefreshToken = newRefreshToken;
	}
	if (!this->appSettingsService.SaveSettings(settings)) {
		LogAuthError("RefreshAccessToken", "Could not save the refreshed token (using it for this request anyway)");
	}
	return accessToken;
}

void GoogleAuthService::SignOut() {
	AppSettings settings = this->appSettingsService.GetSettings();
	settings.calendarApiKey.clear();
	settings.calendarRefreshToken.clear();
	settings.calendarTokenExpiresAt = 0;
	if (!this->appSettingsService.SaveSettings(settings)) {
		LogAuthError("SignOut", "Could not save the cleared tokens");
	}
}

bool GoogleAuthService::IsSignedIn() {
	AppSettings settings = this->appSettingsService.GetSettings();
	return !settings.calendarRefreshToken.empty() || !settings.calendarApiKey.empty();
}
