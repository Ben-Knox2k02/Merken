#include "app_settings_service.h"
#include <fstream>
#include <sstream>

namespace {
	// naive extractor for flat string values, avoids pulling in a JSON library for one settings file
	std::string ExtractValue(const std::string& json, const std::string& key) {
		std::string pattern = "\"" + key + "\"";
		size_t pos = json.find(pattern);
		if(pos == std::string::npos) { return ""; }

		size_t colon = json.find(':', pos + pattern.size());
		if(colon == std::string::npos) { return ""; }

		size_t firstQuote = json.find('"', colon);
		if(firstQuote == std::string::npos) { return ""; }

		size_t secondQuote = json.find('"', firstQuote + 1);
		if(secondQuote == std::string::npos) { return ""; }

		return json.substr(firstQuote + 1, secondQuote - firstQuote - 1);
	}
}

AppSettings AppSettingsService::GetSettings() {
	AppSettings settings;

	std::ifstream file("app_settings.json");
	if(!file.is_open()) { return settings; }

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string json = buffer.str();

	settings.calendarApiKey = ExtractValue(json, "calendarApiKey");
	settings.aiApiKey = ExtractValue(json, "aiApiKey");

	return settings;
}
