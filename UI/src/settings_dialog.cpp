#include "settings_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/SaveAppSettings/save_app_settings_usecase.h"
#include "../../Application/UseCases/Calendar/SignInWithGoogle/sign_in_with_google_usecase.h"
#include "../../Application/UseCases/Calendar/GetGoogleSignInStatus/get_google_sign_in_status_usecase.h"
#include "../../Application/UseCases/Calendar/SignOutOfGoogle/sign_out_of_google_usecase.h"

wxDECLARE_APP(App);

SettingsDialog::SettingsDialog(wxWindow* parent)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		"Settings",
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Gemini API key:"), 0, wxALIGN_CENTER_VERTICAL);
	this->aiKeyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	fields->Add(this->aiKeyCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Google Calendar:"), 0, wxALIGN_CENTER_VERTICAL);
	wxBoxSizer* googleRow = new wxBoxSizer(wxHORIZONTAL);
	this->googleStatusLabel = new wxStaticText(this, wxID_ANY, "Not signed in");
	this->googleButton = new wxButton(this, wxID_ANY, "Sign in with Google");
	googleRow->Add(this->googleStatusLabel, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
	googleRow->Add(this->googleButton, 0);
	fields->Add(googleRow, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Calendar token (manual):"), 0, wxALIGN_CENTER_VERTICAL);
	this->calendarKeyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	fields->Add(this->calendarKeyCtrl, 1, wxEXPAND);

	this->okButton = new wxButton(this, wxID_ANY, "OK");
	this->cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	this->okButton->SetDefault();

	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	buttons->AddStretchSpacer(1);
	buttons->Add(this->okButton, 0, wxRIGHT, 8);
	buttons->Add(this->cancelButton, 0);

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(560, 260));
	this->SetSize(wxSize(600, 280));
	Theme::Get().StyleDialog(this);
	this->Layout();
	this->CentreOnParent();

	this->okButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnSave, this);
	this->googleButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnGoogleButton, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &SettingsDialog::OnClose, this);

	this->RefreshGoogleStatus();
}

SettingsDialog::~SettingsDialog() {
	if (this->signInThread.joinable()) {
		this->signInThread.join();
	}
}

void SettingsDialog::OnSave(wxCommandEvent&) {
	SaveAppSettingsRequest request;
	request.aiApiKey = this->aiKeyCtrl->GetValue().ToStdString();
	request.calendarApiKey = this->calendarKeyCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<SaveAppSettingsUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not save settings.", "Settings", wxOK | wxICON_ERROR);
		return;
	}

	this->EndModal(wxID_OK);
}

void SettingsDialog::OnClose(wxCloseEvent& event) {
	// the sign-in thread reports back to this dialog, so it has to stay open until that finishes
	if (this->signingIn) {
		event.Veto();
		return;
	}
	event.Skip();
}

void SettingsDialog::OnGoogleButton(wxCommandEvent&) {
	if (this->googleSignedIn) {
		this->SignOut();
	} else {
		this->StartSignIn();
	}
}

void SettingsDialog::RefreshGoogleStatus() {
	auto useCase = wxGetApp().GetInjector().create<GetGoogleSignInStatusUseCase>();
	this->googleSignedIn = useCase.Execute().signedIn;
	this->googleStatusLabel->SetLabel(this->googleSignedIn ? "Signed in to Google" : "Not signed in");
	this->googleButton->SetLabel(this->googleSignedIn ? "Sign out" : "Sign in with Google");
	this->Layout();
}

void SettingsDialog::SetBusy(bool busy) {
	this->signingIn = busy;
	this->googleButton->Enable(!busy);
	this->okButton->Enable(!busy);
	this->cancelButton->Enable(!busy);
}

void SettingsDialog::SignOut() {
	auto useCase = wxGetApp().GetInjector().create<SignOutOfGoogleUseCase>();
	useCase.Execute();
	this->RefreshGoogleStatus();
}

void SettingsDialog::StartSignIn() {
	// build the use case here on the UI thread, then hand a copy to the worker
	auto useCase = wxGetApp().GetInjector().create<SignInWithGoogleUseCase>();

	this->SetBusy(true);
	this->googleStatusLabel->SetLabel("Finish signing in in your browser (up to 3 minutes)...");
	this->Layout();

	this->signInThread = std::thread([this, useCase]() mutable {
		const bool ok = useCase.Execute().ok;
		this->CallAfter([this, ok]() { this->OnSignInFinished(ok); });
	});
}

void SettingsDialog::OnSignInFinished(bool ok) {
	if (this->signInThread.joinable()) {
		this->signInThread.join();
	}
	this->SetBusy(false);
	this->RefreshGoogleStatus();
	if (!ok) {
		ShowCenteredMessage(
			this,
			"Google sign-in did not finish. Check that google_client.json is next to Merken.exe and try again.",
			"Google Sign-In",
			wxOK | wxICON_WARNING
		);
	}
}