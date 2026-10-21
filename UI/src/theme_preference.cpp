#include "theme_preference.h"
#include "theme.h"
#include "app.h"
<<<<<<< HEAD
=======
#include "mainframe.h"
>>>>>>> 431e121ef7eafad3a1a7b376066a71e7a2d05be2
#include "../../Application/Services/app_settings_service.h"
#include "../../DomainModels/app_settings.h"

namespace {
<<<<<<< HEAD
void ApplyAppearance(bool dark) {
	Theme::UseDark(dark);
	wxGetApp().SetAppearance(dark ? wxApp::Appearance::Dark : wxApp::Appearance::Light);
=======
void RefreshOpenFrames() {
	for (wxWindowList::compatibility_iterator it = wxTopLevelWindows.GetFirst(); it; it = it.GetNext()) {
		wxWindow* window = it.GetData();
		if (auto* frame = dynamic_cast<MainFrame*>(window)) {
			frame->ApplyTheme();
		} else if (window != nullptr) {
			Theme::ApplyTitleBar(window);
		}
	}
}

void ApplyAppearance(bool dark) {
	Theme::UseDark(dark);
	// SetAppearance only succeeds before the first top-level window, and only
	// while the mode is still the default. Calling it here still updates the
	// custom palette, then the open frames repaint.
	wxGetApp().SetAppearance(dark ? wxApp::Appearance::Dark : wxApp::Appearance::Light);
	RefreshOpenFrames();
>>>>>>> 431e121ef7eafad3a1a7b376066a71e7a2d05be2
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
<<<<<<< HEAD
	}
=======
		return;
	}
	wxGetApp().SetAppearance(wxApp::Appearance::System);
>>>>>>> 431e121ef7eafad3a1a7b376066a71e7a2d05be2
}
