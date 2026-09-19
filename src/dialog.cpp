#include "dialog.h"

CardDialog::CardDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Card", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) { 
	this->SetBackgroundColour(wxColour(*wxWHITE));
	
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
	this->rootSizer->Add(new wxStaticText(this, wxID_ANY, "Front:"), 0, wxALL, 5);
	this->frontCtrl = new wxTextCtrl(this, wxID_ANY);
	this->frontCtrl->SetBackgroundColour(wxColour(*wxWHITE));
	this->frontCtrl->SetForegroundColour(wxColour(*wxBLACK));
	this->rootSizer->Add(this->frontCtrl, 0, wxEXPAND | wxALL, 5);
	
	this->rootSizer->Add(new wxStaticText(this, wxID_ANY, "Back:"), 0, wxALL, 5);
	this->backCtrl = new wxTextCtrl(this, wxID_ANY);
	this->backCtrl->SetBackgroundColour(wxColour(*wxWHITE));
	this->backCtrl->SetForegroundColour(wxColour(*wxBLACK));
	this->rootSizer->Add(this->backCtrl, 0, wxEXPAND | wxALL, 5);
	
	this->rootSizer->Add(new wxStaticText(this, wxID_ANY, "Tag:"), 0, wxALL, 5);
	this->tagCtrl = new wxTextCtrl(this, wxID_ANY);
	this->tagCtrl->SetBackgroundColour(wxColour(*wxWHITE));
	this->tagCtrl->SetForegroundColour(wxColour(*wxBLACK));
	this->rootSizer->Add(this->tagCtrl, 0, wxEXPAND | wxALL, 5);
	
	this->rootSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
	this->SetSizerAndFit(this->rootSizer);
}

DeckDialog::DeckDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Deck", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
	this->SetBackgroundColour(wxColour(*wxWHITE));
	
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
	this->rootSizer->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALL, 5);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY);
	this->nameCtrl->SetBackgroundColour(wxColour(*wxWHITE));
	this->nameCtrl->SetForegroundColour(wxColour(*wxBLACK));
	this->rootSizer->Add(this->nameCtrl, 0, wxEXPAND | wxALL, 5);
	
	this->rootSizer->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALL, 5);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY);
	this->descriptionCtrl->SetBackgroundColour(wxColour(*wxWHITE));
	this->descriptionCtrl->SetForegroundColour(wxColour(*wxBLACK));
	this->rootSizer->Add(this->descriptionCtrl, 0, wxEXPAND | wxALL, 5);

	this->rootSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
	this->SetSizerAndFit(this->rootSizer);
}
