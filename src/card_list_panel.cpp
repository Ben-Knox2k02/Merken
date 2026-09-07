#include "card_list_panel.h"
#include <wx/dataview.h>

CardListPanel::CardListPanel(wxWindow* parent, int deckID) : wxPanel(parent), deckID(deckID) {
    this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(wxColour(*wxWHITE));

    this->header = new wxStaticText(this, wxID_ANY, "Cards in Deck");				 // HEADER
	this->header->SetBackgroundColour(wxColour(*wxWHITE));
	this->header->SetForegroundColour(wxColour(*wxBLACK));
    this->header->SetFont(this->header->GetFont().Bold());
    this->rootSizer->Add(this->header, 0, wxALL, 10);

    this->cardList = new wxDataViewCtrl(this, wxID_ANY);							 // CARD LIST
	this->cardList->SetBackgroundColour(wxColour(*wxBLUE));
	this->cardList->SetForegroundColour(wxColour(*wxWHITE));
    this->cardList->AppendTextColumn("Front", 0, wxDATAVIEW_CELL_INERT, 200);
    this->cardList->AppendTextColumn("Back", 1,  wxDATAVIEW_CELL_INERT, 200);
    this->cardList->AppendTextColumn("Tags", 2,  wxDATAVIEW_CELL_INERT, 150);
	
	this->cardViewModel = new wxDataViewListStore();
	this->cardList->AssociateModel(cardViewModel);
	this->cardViewModel->DecRef();
	
	wxVector<wxVariant> row;
	row.push_back(wxVariant("Hola"));
	row.push_back(wxVariant("Hello"));
	row.push_back(wxVariant("Greeting"));
	this->cardViewModel->AppendItem(row);
	
    this->rootSizer->Add(this->cardList, 1, wxEXPAND | wxALL, 10);

    this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);								 // BUTTONS
	
    this->addButton = new wxButton(this, wxID_ANY, "Add");
	this->addButton->SetBackgroundColour(wxColour(*wxWHITE));
	this->addButton->SetForegroundColour(wxColour(*wxBLACK));
	
    this->editButton = new wxButton(this, wxID_ANY, "Edit");
	this->editButton->SetBackgroundColour(wxColour(*wxWHITE));
	this->editButton->SetForegroundColour(wxColour(*wxBLACK));
	
    this->deleteButton = new wxButton(this, wxID_ANY, "Delete");
	this->deleteButton->SetBackgroundColour(wxColour(*wxWHITE));
	this->deleteButton->SetForegroundColour(wxColour(*wxBLACK));

    this->buttonSizer->Add(this->addButton, 0, 5);
    this->buttonSizer->Add(this->editButton, 0, 5);
    this->buttonSizer->Add(this->deleteButton, 0, 5);

    this->rootSizer->Add(this->buttonSizer, 0, wxALL, 10);
    this->SetSizer(this->rootSizer);
    this->LoadCards();

	this->Bind(wxEVT_PAINT, &CardListPanel::OnPaint, this);
    this->addButton->Bind(wxEVT_BUTTON, &CardListPanel::OnAdd, this);				// BIND EVENTS
    this->editButton->Bind(wxEVT_BUTTON, &CardListPanel::OnEdit, this);
    this->deleteButton->Bind(wxEVT_BUTTON, &CardListPanel::OnDelete, this);
}

void CardListPanel::OnPaint(wxPaintEvent& event) {
	
}

void CardListPanel::LoadCards() {
    // TODO: Load cards from SQLite
}

int CardListPanel::GetSelectedRow() const {
	wxDataViewItem item = this->cardList->GetSelection();
	if(!item.IsOk()) { return -1; }
	else { return this->cardViewModel->GetRow(item);}
}

void CardListPanel::OnAdd(wxCommandEvent& event) {								// ADD CARD
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Add Card"); }
	
	CardDialog dialog(this);
	dialog.CentreOnParent();
	if(dialog.ShowModal() == wxID_OK) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(dialog.frontCtrl->GetValue()));
		row.push_back(wxVariant(dialog.backCtrl->GetValue()));
		row.push_back(wxVariant(dialog.tagCtrl->GetValue()));
		this->cardViewModel->AppendItem(row);
	}
	
}

void CardListPanel::OnEdit(wxCommandEvent& event) {								// EDIT CARD
    wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Card"); }
	
	int row = this->GetSelectedRow();
	if(row == -1) {
		wxMessageBox("No Card Selected", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}
	
	wxVariant front, back, tag;
	wxDataViewItem item = this->cardViewModel->GetItem(row);
	this->cardViewModel->GetValue(front, item, 0);
	this->cardViewModel->GetValue(back, item, 1);
	this->cardViewModel->GetValue(tag, item, 2);
	
	CardDialog dialog(this);
	dialog.CentreOnParent();
	dialog.frontCtrl->SetValue(front.GetString());
	dialog.backCtrl->SetValue(back.GetString());
	dialog.tagCtrl->SetValue(tag.GetString());
	
	if(dialog.ShowModal() == wxID_OK) {
		this->cardViewModel->SetValue(wxVariant(dialog.frontCtrl->GetValue()), item, 0);
		this->cardViewModel->SetValue(wxVariant(dialog.backCtrl->GetValue()), item, 1);
		this->cardViewModel->SetValue(wxVariant(dialog.tagCtrl->GetValue()), item, 2);
	}
	
}

void CardListPanel::OnDelete(wxCommandEvent& event) {							// DELETE CARD
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Card"); }
	
	int row = this->GetSelectedRow();
	if(row == -1) {
		wxMessageBox("No card selected", "Delete Card", wxOK | wxICON_WARNING);
		return;
	}
	else {
		int answer = wxMessageBox("Delete this card?", "Confirm Delete", wxYES_NO | wxICON_QUESTION);
		if(answer == wxYES) {
			wxDataViewItem item = this->cardViewModel->GetItem(row);
			this->cardViewModel->DeleteItem(row);
		}
	}
}
