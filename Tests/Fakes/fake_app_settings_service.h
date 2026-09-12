#ifndef FAKE_APP_SETTINGS_SERVICE_H
#define FAKE_APP_SETTINGS_SERVICE_H

#include "../../Application/ServiceInterfaces/app_settings_service.h"

class FakeAppSettingsService : public IAppSettingsService {
	public:
		AppSettings settings;

		AppSettings GetSettings() override {
			return this->settings;
		}
};

#endif
