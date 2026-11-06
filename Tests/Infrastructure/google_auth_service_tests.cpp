#include "doctest/doctest.h"
#include <string>
#include "../Fakes/fake_http_client.h"
#include "../Fakes/fake_app_settings_service.h"
#include "../Fakes/fake_auth_helpers.h"
#include "../../Infrastructure/GoogleAuth/google_pkce.h"
#include "../../Infrastructure/GoogleAuth/google_auth_service.h"

namespace {
	// reads a value out of a URL or form body (no decoding needed for the values these tests use)
	std::string Param(const std::string& text, const std::string& name) {
		const std::string needle = name + "=";
		size_t start = text.find("?" + needle);
		if (start == std::string::npos) { start = text.find("&" + needle); }
		if (start == std::string::npos) {
			if (text.compare(0, needle.size(), needle) == 0) { start = 0; } else { return ""; }
		} else {
			start += 1;
		}
		start += needle.size();
		const size_t end = text.find('&', start);
		return text.substr(start, end == std::string::npos ? std::string::npos : end - start);
	}

	struct Fixture {
		FakeHttpClient http;
		FakeAppSettingsService settings;
		FakeBrowserLauncher browser;
		FakeLoopbackReceiver receiver;
		FakeClock clock;
		GoogleClientConfig config{"client-id", "client-secret"};

		GoogleAuthService Make() {
			return GoogleAuthService(http, settings, browser, receiver, clock, config);
		}

		// Google redirecting back with a code and the state SignIn sent
		void ApproveWithCode(const std::string& code) {
			receiver.onWait = [this, code]() {
				return "/?code=" + code + "&state=" + Param(browser.lastUrl, "state");
			};
		}
	};

	const char* kTokenBody =
		R"({"access_token":"access-1","refresh_token":"refresh-1","expires_in":3600,"token_type":"Bearer"})";
}

TEST_CASE("GoogleAuthService SignIn opens the browser with a PKCE authorization URL") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, kTokenBody);
	GoogleAuthService service = f.Make();

	REQUIRE(service.SignIn());

	CHECK(f.browser.lastUrl.rfind("https://accounts.google.com/o/oauth2/v2/auth?", 0) == 0);
	CHECK(Param(f.browser.lastUrl, "client_id") == "client-id");
	CHECK(Param(f.browser.lastUrl, "response_type") == "code");
	CHECK(Param(f.browser.lastUrl, "code_challenge_method") == "S256");
	CHECK(Param(f.browser.lastUrl, "access_type") == "offline");
	CHECK(Param(f.browser.lastUrl, "redirect_uri") == "http%3A%2F%2F127.0.0.1%3A54321");
	CHECK(Param(f.browser.lastUrl, "scope") == "https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fcalendar.events");
	CHECK(Param(f.browser.lastUrl, "code_challenge").size() == 43);
}

TEST_CASE("GoogleAuthService SignIn exchanges the code and saves the tokens") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, kTokenBody);
	GoogleAuthService service = f.Make();

	REQUIRE(service.SignIn());

	CHECK(f.http.postFormCallCount == 1);
	CHECK(f.http.lastUrl == "https://oauth2.googleapis.com/token");
	CHECK(Param(f.http.lastBody, "grant_type") == "authorization_code");
	CHECK(Param(f.http.lastBody, "code") == "abc");
	CHECK(Param(f.http.lastBody, "client_id") == "client-id");
	CHECK(Param(f.http.lastBody, "client_secret") == "client-secret");
	CHECK(Param(f.http.lastBody, "redirect_uri") == "http%3A%2F%2F127.0.0.1%3A54321");
	CHECK(f.settings.settings.calendarApiKey == "access-1");
	CHECK(f.settings.settings.calendarRefreshToken == "refresh-1");
	CHECK(f.settings.settings.calendarTokenExpiresAt == 1000000 + 3600);
	CHECK(f.receiver.stopCount == 1);
}

TEST_CASE("GoogleAuthService SignIn sends the verifier that matches the challenge") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, kTokenBody);
	GoogleAuthService service = f.Make();
	REQUIRE(service.SignIn());

	const std::string verifier = Param(f.http.lastBody, "code_verifier");
	CHECK(verifier.size() == 43);
	CHECK(CodeChallengeS256(verifier) == Param(f.browser.lastUrl, "code_challenge"));
}

TEST_CASE("GoogleAuthService SignIn keeps other settings") {
	Fixture f;
	f.settings.settings.aiApiKey = "ai-key";
	f.settings.settings.theme = "dark";
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, kTokenBody);
	GoogleAuthService service = f.Make();

	REQUIRE(service.SignIn());

	CHECK(f.settings.settings.aiApiKey == "ai-key");
	CHECK(f.settings.settings.theme == "dark");
}

TEST_CASE("GoogleAuthService SignIn fails when the client ID is missing") {
	Fixture f;
	f.config.clientId = "";
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.browser.openCount == 0);
	CHECK(f.receiver.startCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails when it cannot listen") {
	Fixture f;
	f.receiver.port = 0;
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.browser.openCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails and stops listening if the browser will not open") {
	Fixture f;
	f.browser.openSucceeds = false;
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.receiver.stopCount == 1);
	CHECK(f.http.postFormCallCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails on timeout") {
	Fixture f;
	f.receiver.onWait = []() { return std::string(); };
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.receiver.stopCount == 1);
	CHECK(f.http.postFormCallCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails when the user denies access") {
	Fixture f;
	f.receiver.onWait = [&f]() { return "/?error=access_denied&state=" + Param(f.browser.lastUrl, "state"); };
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.http.postFormCallCount == 0);
}

TEST_CASE("GoogleAuthService SignIn rejects a redirect with the wrong state") {
	Fixture f;
	f.receiver.onWait = []() { return std::string("/?code=abc&state=forged"); };
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.http.postFormCallCount == 0);
	CHECK(f.settings.saveCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails when the token exchange is rejected") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(400, R"({"error":"invalid_grant","error_description":"Bad code"})");
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.settings.saveCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails when Google sends no refresh token") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, R"({"access_token":"access-1","expires_in":3600})");
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
	CHECK(f.settings.saveCount == 0);
}

TEST_CASE("GoogleAuthService SignIn fails when the tokens cannot be saved") {
	Fixture f;
	f.ApproveWithCode("abc");
	f.http.QueueResponse(200, kTokenBody);
	f.settings.saveSucceeds = false;
	GoogleAuthService service = f.Make();

	CHECK_FALSE(service.SignIn());
}

TEST_CASE("GoogleAuthService GetAccessToken returns the saved token while it is valid") {
	Fixture f;
	f.settings.settings.calendarApiKey = "access-1";
	f.settings.settings.calendarRefreshToken = "refresh-1";
	f.settings.settings.calendarTokenExpiresAt = f.clock.now + 3600;
	GoogleAuthService service = f.Make();

	CHECK(service.GetAccessToken() == "access-1");
	CHECK(f.http.postFormCallCount == 0);
}

TEST_CASE("GoogleAuthService GetAccessToken refreshes an expired token and saves it") {
	Fixture f;
	f.settings.settings.calendarApiKey = "old";
	f.settings.settings.calendarRefreshToken = "refresh-1";
	f.settings.settings.calendarTokenExpiresAt = f.clock.now - 10;
	f.http.QueueResponse(200, R"({"access_token":"access-2","expires_in":3599})");
	GoogleAuthService service = f.Make();

	CHECK(service.GetAccessToken() == "access-2");

	CHECK(Param(f.http.lastBody, "grant_type") == "refresh_token");
	CHECK(Param(f.http.lastBody, "refresh_token") == "refresh-1");
	CHECK(f.settings.settings.calendarApiKey == "access-2");
	CHECK(f.settings.settings.calendarRefreshToken == "refresh-1");
	CHECK(f.settings.settings.calendarTokenExpiresAt == f.clock.now + 3599);
}

TEST_CASE("GoogleAuthService GetAccessToken refreshes a token that is about to expire") {
	Fixture f;
	f.settings.settings.calendarApiKey = "old";
	f.settings.settings.calendarRefreshToken = "refresh-1";
	f.settings.settings.calendarTokenExpiresAt = f.clock.now + 30;
	f.http.QueueResponse(200, R"({"access_token":"access-2","expires_in":3600})");
	GoogleAuthService service = f.Make();

	CHECK(service.GetAccessToken() == "access-2");
}

TEST_CASE("GoogleAuthService GetAccessToken returns empty if the refresh fails") {
	Fixture f;
	f.settings.settings.calendarRefreshToken = "refresh-1";
	f.http.QueueResponse(400, R"({"error":"invalid_grant"})");
	GoogleAuthService service = f.Make();

	CHECK(service.GetAccessToken() == "");
}

TEST_CASE("GoogleAuthService GetAccessToken falls back to a pasted token when there is no refresh token") {
	Fixture f;
	f.settings.settings.calendarApiKey = "pasted";
	GoogleAuthService service = f.Make();

	CHECK(service.GetAccessToken() == "pasted");
	CHECK(f.http.postFormCallCount == 0);
}

TEST_CASE("GoogleAuthService IsSignedIn reflects whether any token is saved") {
	Fixture f;
	GoogleAuthService service = f.Make();
	CHECK_FALSE(service.IsSignedIn());

	f.settings.settings.calendarRefreshToken = "refresh-1";
	CHECK(service.IsSignedIn());
}

TEST_CASE("GoogleAuthService SignOut clears the Google tokens only") {
	Fixture f;
	f.settings.settings.calendarApiKey = "access-1";
	f.settings.settings.calendarRefreshToken = "refresh-1";
	f.settings.settings.calendarTokenExpiresAt = 99;
	f.settings.settings.aiApiKey = "ai-key";
	GoogleAuthService service = f.Make();

	service.SignOut();

	CHECK(f.settings.settings.calendarApiKey == "");
	CHECK(f.settings.settings.calendarRefreshToken == "");
	CHECK(f.settings.settings.calendarTokenExpiresAt == 0);
	CHECK(f.settings.settings.aiApiKey == "ai-key");
	CHECK_FALSE(service.IsSignedIn());
}