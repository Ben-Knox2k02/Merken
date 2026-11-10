#include "settings_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/Services/app_settings_service.h"
#include "../../Application/UseCases/SaveAppSettings/save_app_settings_usecase.h"
#include "../../Application/UseCases/Calendar/GetGoogleSignInStatus/get_google_sign_in_status_usecase.h"
#include "../../Application/UseCases/Calendar/SignInWithGoogle/sign_in_with_google_usecase.h"
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
	),
	  alive(std::make_shared<bool>(true)),
	  signingIn(false),
	  googleSignedIn(false) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Gemini API key:"), 0, wxALIGN_CENTER_VERTICAL);
	this->aiKeyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	fields->Add(this->aiKeyCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Calendar API key:"), 0, wxALIGN_CENTER_VERTICAL);
	this->calendarKeyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	fields->Add(this->calendarKeyCtrl, 1, wxEXPAND);

	auto& service = wxGetApp().GetInjector().create<IAppSettingsService&>();
	const AppSettings settings = service.GetSettings();
	this->aiKeyCtrl->ChangeValue(wxString::FromUTF8(settings.aiApiKey.c_str()));
	this->calendarKeyCtrl->ChangeValue(wxString::FromUTF8(settings.calendarApiKey.c_str()));

	this->googleButton = new wxButton(this, wxID_ANY, "Sign in with Google");
	ApplyButtonPadding(this->googleButton);
	this->statusLabel = new wxStaticText(this, wxID_ANY, "Signed in to Google.");
	wxBoxSizer* googleRow = new wxBoxSizer(wxHORIZONTAL);
	googleRow->Add(this->googleButton, 0, wxALIGN_CENTER_VERTICAL);
	googleRow->Add(this->statusLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 12);

	wxStaticText* note = new wxStaticText(this, wxID_ANY, "google_client.json must sit next to Merken.exe.");

	this->okButton = new wxButton(this, wxID_ANY, "OK");
	this->cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	ApplyButtonPadding(this->okButton);
	ApplyButtonPadding(this->cancelButton);
	this->okButton->SetDefault();

	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	buttons->AddStretchSpacer(1);
	buttons->Add(this->okButton, 0, wxRIGHT, 8);
	buttons->Add(this->cancelButton, 0);

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 0, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(googleRow, 0, wxLEFT | wxRIGHT | wxBOTTOM, 16);
	this->rootSizer->Add(note, 0, wxLEFT | wxRIGHT | wxBOTTOM, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->Fit();
	const int width = this->FromDIP(480);
	wxSize size = this->GetSize();
	if (size.GetWidth() < width) {
		size.SetWidth(width);
	}
	this->SetMinSize(size);
	this->SetSize(size);
	Theme::Get().StyleDialog(this);

	auto status = wxGetApp().GetInjector().create<GetGoogleSignInStatusUseCase>();
	this->ApplySignInState(status.Execute().signedIn);
	this->Layout();
	this->CentreOnParent();

	this->okButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnSave, this);
	this->googleButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnGoogle, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &SettingsDialog::OnClose, this);
}

SettingsDialog::~SettingsDialog() {
	*this->alive = false;
	if (!this->worker.joinable()) {
		return;
	}
	// Sign-in waits on the UI thread for HTTP. Joining here would deadlock.
	if (this->signingIn) {
		this->worker.detach();
		return;
	}
	this->worker.join();
}

void SettingsDialog::JoinWorker() {
	if (this->worker.joinable()) {
		this->worker.join();
	}
}

void SettingsDialog::ApplySignInState(bool signedIn) {
	this->googleSignedIn = signedIn;
	if (signedIn) {
		this->statusLabel->SetLabel("Signed in to Google.");
		this->googleButton->SetLabel("Sign out");
		return;
	}
	this->statusLabel->SetLabel("Not signed in.");
	this->googleButton->SetLabel("Sign in with Google");
}

void SettingsDialog::OnSave(wxCommandEvent&) {
	if (this->signingIn) {
		return;
	}

	SaveAppSettingsRequest request;
	request.aiApiKey = this->aiKeyCtrl->GetValue().ToStdString(wxConvUTF8);
	request.calendarApiKey = this->calendarKeyCtrl->GetValue().ToStdString(wxConvUTF8);

	auto useCase = wxGetApp().GetInjector().create<SaveAppSettingsUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not save settings.", "Settings", wxOK | wxICON_ERROR);
		return;
	}

	this->EndModal(wxID_OK);
}

void SettingsDialog::OnGoogle(wxCommandEvent&) {
	if (this->signingIn) {
		return;
	}
	this->JoinWorker();

	const bool signOut = this->googleSignedIn;
	this->signingIn = true;
	this->googleButton->Enable(false);
	this->okButton->Enable(false);
	this->cancelButton->Enable(false);
	this->statusLabel->SetLabel(signOut ? "Signing out..." : "Signing in...");
	this->Layout();

	auto aliveFlag = this->alive;
	if (signOut) {
		auto useCase = wxGetApp().GetInjector().create<SignOutOfGoogleUseCase>();
		this->worker = std::thread([this, aliveFlag, useCase]() mutable {
			useCase.Execute();
			if (wxTheApp == nullptr) {
				return;
			}
			wxTheApp->CallAfter([this, aliveFlag]() {
				if (!*aliveFlag) {
					return;
				}
				this->OnAuthFinished(false, true);
			});
		});
		return;
	}

	auto useCase = wxGetApp().GetInjector().create<SignInWithGoogleUseCase>();
	this->worker = std::thread([this, aliveFlag, useCase]() mutable {
		const bool ok = useCase.Execute().ok;
		if (wxTheApp == nullptr) {
			return;
		}
		wxTheApp->CallAfter([this, aliveFlag, ok]() {
			if (!*aliveFlag) {
				return;
			}
			this->OnAuthFinished(!ok, false);
		});
	});
}

void SettingsDialog::OnAuthFinished(bool failed, bool fromSignOut) {
	this->signingIn = false;
	this->googleButton->Enable(true);
	this->okButton->Enable(true);
	this->cancelButton->Enable(true);
	auto status = wxGetApp().GetInjector().create<GetGoogleSignInStatusUseCase>();
	const bool signedIn = status.Execute().signedIn;
	if (fromSignOut && !signedIn) {
		this->calendarKeyCtrl->ChangeValue(wxString());
	}
	this->ApplySignInState(signedIn);
	this->Layout();
	if (failed && !signedIn) {
		ShowCenteredMessage(
			this,
			"Google sign-in failed. Put google_client.json next to Merken.exe.",
			"Settings",
			wxOK | wxICON_ERROR
		);
	}
}

void SettingsDialog::OnClose(wxCloseEvent& event) {
	if (this->signingIn) {
		event.Veto();
		return;
	}
	event.Skip();
}
