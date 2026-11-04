#include "save_app_settings_usecase.h"

bool SaveAppSettingsUseCase::Execute(const SaveAppSettingsRequest& request) {
	AppSettings settings = this->appSettingsService.GetSettings();
	if (!request.calendarApiKey.empty()) {
		settings.calendarApiKey = request.calendarApiKey;
	}
	settings.aiApiKey = request.aiApiKey;
	return this->appSettingsService.SaveSettings(settings);
}
