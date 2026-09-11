#include "card_dialog.h"

CardDialog::CardDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Card") {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 8, 8);
	fields->AddGrowableCol(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Front:"), 0, wxALIGN_CENTER_VERTICAL);
	this->frontCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->frontCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Back:"), 0, wxALIGN_CENTER_VERTICAL);
	this->backCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->backCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALIGN_CENTER_VERTICAL);
	this->tagCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->tagCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 12);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);

	this->SetSizerAndFit(this->rootSizer);
	this->SetMinSize(this->GetSize());
}
