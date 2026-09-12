#include "card_list_panel.h"
#include "centered_message.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"

wxDECLARE_APP(App);

CardListPanel::CardListPanel(wxWindow* parent, int deckID) : wxPanel(parent), deckID(deckID) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Cards in Deck");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->cardList = new wxDataViewCtrl(this, wxID_ANY);
	this->cardList->AppendTextColumn("Front", 0, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	this->cardList->AppendTextColumn("Back", 1, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);
	this->cardList->AppendTextColumn("Tags", 2, wxDATAVIEW_CELL_INERT, 300, wxALIGN_CENTER);

	this->cardViewModel = new wxDataViewListStore();
	this->cardList->AssociateModel(this->cardViewModel);
	this->cardViewModel->DecRef();

	this->rootSizer->Add(this->cardList, 1, wxEXPAND | wxALL, 10);

	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->addButton = new wxButton(this, wxID_ANY, "Add");
	this->editButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteButton = new wxButton(this, wxID_ANY, "Delete");

	this->buttonSizer->Add(this->addButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->editButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->deleteButton, 0);

	this->rootSizer->Add(this->buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);
	this->SetSizer(this->rootSizer);
	this->LoadCards();

	this->addButton->Bind(wxEVT_BUTTON, &CardListPanel::OnAdd, this);
	this->editButton->Bind(wxEVT_BUTTON, &CardListPanel::OnEdit, this);
	this->deleteButton->Bind(wxEVT_BUTTON, &CardListPanel::OnDelete, this);
}

void CardListPanel::SetDeck(int deckId) {
	this->deckID = deckId;
	this->LoadCards();
}

void CardListPanel::LoadCards() {
	this->cardViewModel->DeleteAllItems();
	if (this->deckID == 0) { return; }

	// Retrieve the list of cards for the current deck using the GetCardsUseCase.
	auto useCase = wxGetApp().GetInjector().create<GetCardsUseCase>();
	GetCardsResponse response = useCase.Execute(GetCardsRequest{this->deckID});

	for (const CardResponse& card : response.cards) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(wxString(card.front)));
		row.push_back(wxVariant(wxString(card.back)));
		row.push_back(wxVariant(wxString(card.tags)));
		this->cardViewModel->AppendItem(row, static_cast<wxUIntPtr>(card.cardId));
	}
}

int CardListPanel::GetSelectedRow() const {
	wxDataViewItem item = this->cardList->GetSelection();
	if (!item.IsOk()) { return -1; }
	return static_cast<int>(this->cardViewModel->GetRow(item));
}

int CardListPanel::GetSelectedCardId() const {
	wxDataViewItem item = this->cardList->GetSelection();
	if (!item.IsOk()) { return 0; }
	return static_cast<int>(this->cardViewModel->GetItemData(item));
}

void CardListPanel::OnAdd(wxCommandEvent&) {
	if (this->deckID == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	CardDialog dialog(this, "Add Card");
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string front = dialog.frontCtrl->GetValue().ToStdString();
	const std::string back = dialog.backCtrl->GetValue().ToStdString();
	if (front.empty() || back.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	// Prepare the request to create a new card.
	CreateCardRequest request;
	request.deckId = this->deckID;
	request.front = front;
	request.back = back;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	// Call the CreateCardUseCase to create a new card.
	auto useCase = wxGetApp().GetInjector().create<CreateCardUseCase>();
	CreateCardResponse response = useCase.Execute(request);
	if (response.cardId == 0) {
		ShowCenteredMessage(this, "Could not save card.", "Add Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void CardListPanel::OnEdit(wxCommandEvent&) {
	const int cardId = this->GetSelectedCardId();
	if (cardId == 0) {
		ShowCenteredMessage(this, "No card selected.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	int row = this->GetSelectedRow();
	wxDataViewItem item = this->cardViewModel->GetItem(row);
	wxVariant front, back, tag;
	this->cardViewModel->GetValue(front, item, 0);
	this->cardViewModel->GetValue(back, item, 1);
	this->cardViewModel->GetValue(tag, item, 2);

	CardDialog dialog(this, "Edit Card");
	dialog.frontCtrl->SetValue(front.GetString());
	dialog.backCtrl->SetValue(back.GetString());
	dialog.tagCtrl->SetValue(tag.GetString());
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string newFront = dialog.frontCtrl->GetValue().ToStdString();
	const std::string newBack = dialog.backCtrl->GetValue().ToStdString();
	if (newFront.empty() || newBack.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}
	// Prepare the request to update the card with the new information.
	UpdateCardRequest request;
	request.deckId = this->deckID;
	request.cardId = cardId;
	request.front = newFront;
	request.back = newBack;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	// Call the UpdateCardUseCase to update the card with the new information.
	auto useCase = wxGetApp().GetInjector().create<UpdateCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not update card.", "Edit Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void CardListPanel::OnDelete(wxCommandEvent&) {
	ShowCenteredMessage(this, "Delete is not implemented yet.", "Delete Card", wxOK | wxICON_INFORMATION);
}
