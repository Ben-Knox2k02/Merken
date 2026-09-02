#include "deck_panel.h"
#include <wx/wx.h>
#include <wx/dataview.h>

DeckPanel::DeckPanel(wxWindow* parent) : wxPanel(parent) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(wxColour(240, 240, 240));
	this->SetMinSize(wxSize(240, -1));
	
	this->header = new wxStaticText(this, wxID_ANY, "Decks");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
	
	this->deckList = new wxDataViewCtrl(this, wxID_ANY);
	this->deckList->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);
	
	auto* model = new wxDataViewListStore();
	this->deckList->AssociateModel(model);
	model->DecRef();
	
	wxVector<wxVariant> row;
	row.push_back(wxVariant("Spanish"));
	model->AppendItem(row);
	
    this->rootSizer->Add(this->deckList, 1, wxEXPAND | wxALL, 10);
	
	this->addDeckButton = new wxButton(this, wxID_ANY, "Add");
	this->editDeckButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteDeckButton = new wxButton(this, wxID_ANY, "Delete");
	
	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->buttonSizer->Add(this->addDeckButton, 1, wxRIGHT, 5);
	this->buttonSizer->Add(this->editDeckButton, 1, wxRIGHT, 5);
	this->buttonSizer->Add(this->deleteDeckButton, 1);
	
	this->rootSizer->Add(this->buttonSizer, 0, wxEXPAND | wxALL, 10);
	this->SetSizer(this->rootSizer);
	this->LoadDecks();
	
	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnAddDeck, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnEditDeck, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnDeleteDeck, this);
	this->deckList->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DeckPanel::OnDeckActivated, this);
}

void DeckPanel::LoadDecks() {

}

void DeckPanel::OnAddDeck(wxCommandEvent& event) {
	
}

void DeckPanel::OnEditDeck(wxCommandEvent& event) {
	
}

void DeckPanel::OnDeleteDeck(wxCommandEvent& event) {
	
}

void DeckPanel::OnDeckActivated(wxDataViewEvent& event) {

}

