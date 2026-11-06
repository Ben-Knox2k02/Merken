#include "google_client_config.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include <fstream>
#include <sstream>

static std::string ReadString(const nlohmann::json& obj, const char* key) {
	if (obj.is_object() && obj.contains(key) && obj[key].is_string()) {
		return obj[key].get<std::string>();
	}
	return "";
}

GoogleClientConfig ParseGoogleClientConfig(const std::string& text) {
	GoogleClientConfig config;
	nlohmann::json json = nlohmann::json::parse(text, nullptr, false);
	if (!json.is_object()) {
		return config;
	}

	const nlohmann::json* source = &json;
	if (json.contains("installed") && json["installed"].is_object()) {
		source = &json["installed"];
	}

	config.clientId = ReadString(*source, "clientId");
	if (config.clientId.empty()) {
		config.clientId = ReadString(*source, "client_id");
	}
	config.clientSecret = ReadString(*source, "clientSecret");
	if (config.clientSecret.empty()) {
		config.clientSecret = ReadString(*source, "client_secret");
	}
	return config;
}

GoogleClientConfig LoadGoogleClientConfig(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return GoogleClientConfig{};
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return ParseGoogleClientConfig(buffer.str());
}