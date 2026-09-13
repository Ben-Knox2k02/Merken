#ifndef APP_SETTINGS_SERVICE_REAL_H
#define APP_SETTINGS_SERVICE_REAL_H

#include "../../Application/Services/app_settings_service.h"

class AppSettingsService : public IAppSettingsService {
	public:
		AppSettings GetSettings() override;
};

#endif
