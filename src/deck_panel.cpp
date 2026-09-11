#include "deck_panel.h"

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
	this->deckList->AppendTextColumn("Description", 1, wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);
	
	this->deckViewModel = new wxDataViewListStore();
	this->deckList->AssociateModel(deckViewModel);
	this->deckViewModel->DecRef();
	
	wxVector<wxVariant> row;							// HARD-CODED DEMO DECK
	row.push_back(wxVariant("Spanish"));
	row.push_back(wxVariant("Spanish language"));
	this->deckViewModel->AppendItem(row);
	
	Deck spanishDeck;
	spanishDeck.ID = 0;
	spanishDeck.name = "Spanish";
	spanishDeck.description = "Spanish language";
	this->decks.push_back(spanishDeck);
	
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
	
	//============= BIND EVENT HANDLERS =========================================================
	
	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnAdd, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnEdit, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnDelete, this);
	this->deckList->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DeckPanel::OnDeckActivated, this);
	
	this->Print();
}

void DeckPanel::Print() {
	std::cout<<"--------------------------------------\n";
	std::cout<<"Decks.size(): "<<this->decks.size()<<"\n";
	
	for(auto& deck : this->decks) {
		std::cout<<"Deck ID: "<<deck.ID<<"\nName: "<<deck.name<<"\nDescription: "<<deck.description<<"\n\n"; 
	}
	
	std::cout<<"--------------------------------------\n\n";
}

void DeckPanel::LoadDecks() {
	// TODO: LOAD DECK DATA FROM SQLite
}


int DeckPanel::GetSelectedRow() const {
	wxDataViewItem item = this->deckList->GetSelection();
	if(!item.IsOk()) { return -1; }
	else { return this->deckViewModel->GetRow(item);}
}

void DeckPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(this->GetClientRect());
}

void DeckPanel::OnAdd(wxCommandEvent& event) {									// ADD DECK
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Add Deck"); }
	
	DeckDialog dialog(this);
	dialog.CentreOnParent();
	
	if(dialog.ShowModal() == wxID_OK) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(dialog.nameCtrl->GetValue()));
		row.push_back(wxVariant(dialog.descriptionCtrl->GetValue()));
		this->deckViewModel->AppendItem(row);
		
		Deck newDeck;
		newDeck.ID = decks.size();
		newDeck.name = dialog.nameCtrl->GetValue();
		newDeck.description = dialog.descriptionCtrl->GetValue();
		this->decks.push_back(newDeck);
	}
	
	this->Print();
	event.Skip();		// PROPAGATE EVENTS
}

void DeckPanel::OnEdit(wxCommandEvent& event) {									// EDIT DECK
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Deck"); }
	
	int row = this->GetSelectedRow();
	if(row == -1) {
		wxMessageBox("No Deck Selected", "Edit Deck", wxOK | wxICON_WARNING);
		return;
	}
	
	wxVariant name, description;
	wxDataViewItem item = this->deckViewModel->GetItem(row);
	this->deckViewModel->GetValue(name, item, 0);
	this->deckViewModel->GetValue(description, item, 1);
	
	DeckDialog dialog(this);
	dialog.CentreOnParent();
	dialog.nameCtrl->SetValue(name.GetString());
	dialog.descriptionCtrl->SetValue(description.GetString());
	
	if(dialog.ShowModal() == wxID_OK) {
		this->deckViewModel->SetValue(wxVariant(dialog.nameCtrl->GetValue()), item, 0);
		this->deckViewModel->SetValue(wxVariant(dialog.descriptionCtrl->GetValue()), item, 1);
		
		this->decks[row].name = dialog.nameCtrl->GetValue();
		this->decks[row].description = dialog.descriptionCtrl->GetValue();
	}
	
	this->Print();
	event.Skip();
}

void DeckPanel::OnDelete(wxCommandEvent& event) {								// DELETE DECK
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Deck"); }
	
	int row = this->GetSelectedRow();
	if(row == -1) {
		wxMessageBox("No Deck Selected", "Delete Deck", wxOK | wxICON_WARNING);
		return;
	}
	else {
		int answer = wxMessageBox("Delete this deck?", "Confirm Delete", wxYES_NO | wxICON_QUESTION);
		if(answer == wxYES) {
			this->deckViewModel->DeleteItem(row);
			this->decks.erase(decks.begin() + row);
			for(int i = 0; i < this->decks.size(); i++) { this->decks[i].ID = i; }	// RENUMBER IDS TO MATCH NEW INDICES
		}
	}
	
	this->Print();
	event.Skip();
}

void DeckPanel::OnDeckActivated(wxDataViewEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Deck Activated"); }
	
	
	event.Skip();
}

