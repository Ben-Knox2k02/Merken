#include "card_list_panel.h"
#include <wx/wx.h>
#include <wx/dataview.h>

CardListPanel::CardListPanel(wxWindow* parent, int deckID) : wxPanel(parent), deckID(deckID) {
    this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(*wxWHITE);

    this->header = new wxStaticText(this, wxID_ANY, "Cards in Deck");				 // HEADER
    this->header->SetFont(this->header->GetFont().Bold());
    this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

    this->cardList = new wxDataViewCtrl(this, wxID_ANY);							 // CARD LIST
    this->cardList->AppendTextColumn("Front", 0, wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);
    this->cardList->AppendTextColumn("Back", 1,  wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);
    this->cardList->AppendTextColumn("Tags", 2,  wxDATAVIEW_CELL_INERT, 150, wxALIGN_CENTER);
	
	auto* model = new wxDataViewListStore();
	this->cardList->AssociateModel(model);
	model->DecRef();
	
	wxVector<wxVariant> row;
	row.push_back(wxVariant("Hola"));
	row.push_back(wxVariant("Hello"));
	row.push_back(wxVariant("Greeting"));
	model->AppendItem(row);
	
    this->rootSizer->Add(this->cardList, 1, wxEXPAND | wxALL, 10);

    this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);								 // BUTTONS
    this->addButton = new wxButton(this, wxID_ANY, "Add");
    this->editButton = new wxButton(this, wxID_ANY, "Edit");
    this->deleteButton = new wxButton(this, wxID_ANY, "Delete");

    this->buttonSizer->Add(this->addButton, 0, wxRIGHT, 5);
    this->buttonSizer->Add(this->editButton, 0, wxRIGHT, 5);
    this->buttonSizer->Add(this->deleteButton, 0);

    this->rootSizer->Add(this->buttonSizer, 0, wxALIGN_RIGHT | wxALL, 10);
    this->SetSizer(this->rootSizer);
    this->LoadCards();

    this->addButton->Bind(wxEVT_BUTTON, &CardListPanel::OnAdd, this);				// BIND EVENTS
    this->editButton->Bind(wxEVT_BUTTON, &CardListPanel::OnEdit, this);
    this->deleteButton->Bind(wxEVT_BUTTON, &CardListPanel::OnDelete, this);
}

void CardListPanel::LoadCards() {
    // TODO: Load cards from SQLite
}

void CardListPanel::OnAdd(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Add Card"); }
}

void CardListPanel::OnEdit(wxCommandEvent& event) {
    wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Card"); }
}

void CardListPanel::OnDelete(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Card"); }
}
