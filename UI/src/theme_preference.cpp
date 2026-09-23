#include "theme_preference.h"
#include "theme.h"
#include "app.h"
#include "../../Application/Services/app_settings_service.h"
#include "../../DomainModels/app_settings.h"

wxDECLARE_APP(App);

namespace {
void ApplyAppearance(bool dark) {
	Theme::UseDark(dark);
	wxGetApp().SetAppearance(dark ? wxApp::Appearance::Dark : wxApp::Appearance::Light);
}
}

bool SaveAndApplyTheme(bool dark) {
	auto& service = wxGetApp().GetInjector().create<IAppSettingsService&>();
	AppSettings settings = service.GetSettings();
	settings.theme = dark ? "dark" : "light";
	if (!service.SaveSettings(settings)) {
		return false;
	}
	ApplyAppearance(dark);
	return true;
}

void ApplyStoredTheme() {
	auto& service = wxGetApp().GetInjector().create<IAppSettingsService&>();
	const std::string theme = service.GetSettings().theme;
	if (theme == "dark") {
		ApplyAppearance(true);
		return;
	}
	if (theme == "light") {
		ApplyAppearance(false);
	}
}
