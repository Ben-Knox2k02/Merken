#ifndef APP_SETTINGS_SERVICE_H
#define APP_SETTINGS_SERVICE_H

#include "../../DomainModels/app_settings.h"

class IAppSettingsService {
	public:
		virtual ~IAppSettingsService() = default;

		virtual AppSettings GetSettings() = 0;
		virtual bool SaveSettings(const AppSettings& settings) = 0;
};

#endif
