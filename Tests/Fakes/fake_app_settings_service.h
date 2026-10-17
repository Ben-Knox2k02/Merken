#ifndef FAKE_APP_SETTINGS_SERVICE_H
#define FAKE_APP_SETTINGS_SERVICE_H

#include "../../Application/Services/app_settings_service.h"

class FakeAppSettingsService : public IAppSettingsService {
	public:
		AppSettings settings;
		int saveCount = 0;
		bool saveSucceeds = true;

		AppSettings GetSettings() override {
			return this->settings;
		}

		bool SaveSettings(const AppSettings& settings) override {
			if (!this->saveSucceeds) {
				return false;
			}
			this->settings = settings;
			this->saveCount += 1;
			return true;
		}
};

#endif
