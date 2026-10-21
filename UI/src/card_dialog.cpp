#include "card_dialog.h"

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
