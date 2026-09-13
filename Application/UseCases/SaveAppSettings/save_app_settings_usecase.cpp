#include "save_app_settings_usecase.h"

bool SaveAppSettingsUseCase::Execute(const SaveAppSettingsRequest& request) {
	AppSettings settings;
	settings.calendarApiKey = request.calendarApiKey;
	settings.aiApiKey = request.aiApiKey;
	return this->appSettingsService.SaveSettings(settings);
}
