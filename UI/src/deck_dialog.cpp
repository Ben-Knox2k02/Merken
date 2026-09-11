#include "deck_dialog.h"

DeckDialog::DeckDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Deck") {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 8, 8);
	fields->AddGrowableCol(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->nameCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_CENTER_VERTICAL);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->descriptionCtrl->SetMinSize(wxSize(-1, 60));
	fields->Add(this->descriptionCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 12);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

	this->SetSizerAndFit(this->rootSizer);
	this->SetMinSize(wxSize(360, this->GetSize().GetHeight()));
}
