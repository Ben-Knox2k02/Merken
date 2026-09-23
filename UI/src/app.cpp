#include "app.h"
#include "mainframe.h"
#include "onboarding_dialog.h"
#include "theme_preference.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"

void App::EnsureInjector() {
	if (!this->injector.has_value()) {
		this->injector.emplace(MakeInjector());
	}
}

bool App::OnInit() {
	this->SetAppName("Merken");
	this->SetAppDisplayName("Merken");
	this->SetAppearance(Appearance::System);
	wxInitAllImageHandlers();
	this->EnsureInjector();
	ApplyStoredTheme();

	auto profileUseCase = this->GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = profileUseCase.Execute();
	if (profile.ok && !profile.onboardingFinished) {
		OnboardingDialog onboarding(nullptr);
		onboarding.ShowModal();
	}

	MainFrame* mainFrame = new MainFrame("Merken");
	const wxSize initialSize = mainFrame->FromDIP(wxSize(1366, 768));
	mainFrame->SetClientSize(initialSize);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}

int App::OnExit() {
	this->injector.reset();
	return wxApp::OnExit();
}

wxIMPLEMENT_APP(App);
