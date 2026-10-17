#include "doctest/doctest.h"
#include "../../../Application/UseCases/SaveAppSettings/save_app_settings_usecase.h"
#include "../../Fakes/fake_app_settings_service.h"

TEST_CASE("SaveAppSettingsUseCase stores API keys") {
	FakeAppSettingsService appSettings;
	SaveAppSettingsUseCase useCase(appSettings);

	SaveAppSettingsRequest request;
	request.calendarApiKey = "cal-key";
	request.aiApiKey = "ai-key";

	CHECK(useCase.Execute(request));
	CHECK(appSettings.saveCount == 1);
	CHECK(appSettings.settings.calendarApiKey == "cal-key");
	CHECK(appSettings.settings.aiApiKey == "ai-key");
}

TEST_CASE("SaveAppSettingsUseCase keeps the saved theme") {
	FakeAppSettingsService appSettings;
	appSettings.settings.theme = "dark";
	SaveAppSettingsUseCase useCase(appSettings);

	SaveAppSettingsRequest request;
	request.calendarApiKey = "cal-key";
	request.aiApiKey = "ai-key";

	CHECK(useCase.Execute(request));
	CHECK(appSettings.settings.theme == "dark");
	CHECK(appSettings.settings.aiApiKey == "ai-key");
}

TEST_CASE("SaveAppSettingsUseCase returns false when save fails") {
	FakeAppSettingsService appSettings;
	appSettings.saveSucceeds = false;
	SaveAppSettingsUseCase useCase(appSettings);

	SaveAppSettingsRequest request;
	request.aiApiKey = "ai-key";

	CHECK_FALSE(useCase.Execute(request));
	CHECK(appSettings.saveCount == 0);
	CHECK(appSettings.settings.aiApiKey.empty());
}
