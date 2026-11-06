#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <string>

struct AppSettings {
	std::string calendarApiKey;
	std::string calendarRefreshToken;
	long long calendarTokenExpiresAt = 0;
	std::string aiApiKey;
	std::string theme;
};

#endif