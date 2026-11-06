#include "settings_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/SaveAppSettings/save_app_settings_usecase.h"

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

	fields->Add(new wxStaticText(this, wxID_ANY, "Calendar API key:"), 0, wxALIGN_CENTER_VERTICAL);
	this->calendarKeyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	fields->Add(this->calendarKeyCtrl, 1, wxEXPAND);

	wxButton* okButton = new wxButton(this, wxID_ANY, "OK");
	wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	okButton->SetDefault();

	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	buttons->AddStretchSpacer(1);
	buttons->Add(okButton, 0, wxRIGHT, 8);
	buttons->Add(cancelButton, 0);

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(480, 200));
	this->SetSize(wxSize(520, 220));
	Theme::Get().StyleDialog(this);
	this->Layout();
	this->CentreOnParent();

	okButton->Bind(wxEVT_BUTTON, &SettingsDialog::OnSave, this);
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
