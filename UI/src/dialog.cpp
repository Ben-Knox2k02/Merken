#include "dialog.h"

CardDialog::CardDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(0, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Front:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->frontCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->frontCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->frontCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Back:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->backCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->backCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->backCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALIGN_CENTER_VERTICAL);
	this->tagCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->tagCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(480, 360));
	this->SetSize(wxSize(520, 400));
	this->Layout();
	this->CentreOnParent();
}

DeckDialog::DeckDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->nameCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->descriptionCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->descriptionCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(480, 320));
	this->SetSize(wxSize(520, 360));
	this->Layout();
	this->CentreOnParent();
}

EventDialog::EventDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Create Event", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
	this->SetBackgroundColour(wxColour(*wxGREEN));
	
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
	this->titleText = new wxStaticText(this, wxID_ANY, "Title:");
	this->titleCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->titleText, 0, wxLEFT | wxALL, 5);
	this->rootSizer->Add(this->titleCtrl, 0, wxRIGHT | wxALL, 5);
	
	this->dateText = new wxStaticText(this, wxID_ANY, "Date:");
	this->dateCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->dateText, 0, wxLEFT | wxALL, 5);
	this->rootSizer->Add(this->dateCtrl, 0, wxRIGHT | wxALL, 5);
	
	this->locationText = new wxStaticText(this, wxID_ANY, "Location:");
	this->locationCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->locationText, 0, wxLEFT | wxALL, 5);
	this->rootSizer->Add(this->locationCtrl, 0, wxRIGHT | wxALL, 5);
	
	this->descriptionText = new wxStaticText(this, wxID_ANY, "Description:");
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->descriptionText, 0, wxLEFT | wxALL, 5);
	this->rootSizer->Add(this->descriptionCtrl, 0, wxRIGHT | wxALL, 5);
	
	this->reminderText = new wxStaticText(this, wxID_ANY, "Reminder:");
	this->reminderCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->reminderText, 0, wxLEFT | wxALL, 5);
	this->rootSizer->Add(this->reminderCtrl, 0, wxRIGHT | wxALL, 5);
	
	this->rootSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
	this->SetSizerAndFit(this->rootSizer);
}
