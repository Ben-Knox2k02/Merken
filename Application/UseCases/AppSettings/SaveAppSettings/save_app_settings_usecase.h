#ifndef SAVE_APP_SETTINGS_USE_CASE_H
#define SAVE_APP_SETTINGS_USE_CASE_H

#include "save_app_settings_request.h"
#include "../../../Services/app_settings_service.h"

class SaveAppSettingsUseCase {
	public:
		explicit SaveAppSettingsUseCase(IAppSettingsService& appSettingsService)
			: appSettingsService(appSettingsService) {}

		bool Execute(const SaveAppSettingsRequest& request);

	private:
		IAppSettingsService& appSettingsService;
};

#endif
