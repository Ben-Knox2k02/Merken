#include "flash_card_list.h"
#include "card_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"

wxDECLARE_APP(App);

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  selectedCardId(0) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Cards in Deck");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxALL, 10);

	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->addButton = new wxButton(this, wxID_ANY, "Add");
	this->editButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteButton = new wxButton(this, wxID_ANY, "Delete");
	this->studyButton = new wxButton(this, wxID_ANY, "Study Deck");
	this->buttonSizer->Add(this->addButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->editButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->deleteButton, 0, wxRIGHT, 5);
	this->buttonSizer->Add(this->studyButton, 0);
	this->rootSizer->Add(this->buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

	wxBoxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);
	outerSizer->Add(this->rootSizer, 1, wxEXPAND | wxALL, kMargin);
	this->SetSizer(outerSizer);

	this->addButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAdd, this);
	this->editButton->Bind(wxEVT_BUTTON, &FlashCardList::OnEdit, this);
	this->deleteButton->Bind(wxEVT_BUTTON, &FlashCardList::OnDelete, this);
	this->studyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnStudy, this);
	this->Bind(wxEVT_PAINT, &FlashCardList::OnPaint, this);
	this->Bind(wxEVT_SIZE, &FlashCardList::OnSize, this);

	this->LoadCards();
}

void FlashCardList::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	const wxSize size = this->GetClientSize();
	if (size.GetWidth() < 2 || size.GetHeight() < 2) {
		return;
	}
	const int margin = kMargin;
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1));
	dc.DrawRoundedRectangle(
		margin,
		margin,
		size.GetWidth() - 2 * margin - 1,
		size.GetHeight() - 2 * margin - 1,
		kCornerRadius
	);
}

void FlashCardList::OnSize(wxSizeEvent& event) {
	this->scroller->Layout();
	this->scroller->FitInside();
	this->Refresh();
	event.Skip();
}

void FlashCardList::SetDeck(int deckId) {
	this->deckId = deckId;
	this->LoadCards();
}

void FlashCardList::LoadCards() {
	const int previouslySelected = this->GetSelectedCardId();
	this->listSizer->Clear(true);
	this->cards.clear();
	this->cardIds.clear();

	if (this->deckId != 0) {
		auto useCase = wxGetApp().GetInjector().create<GetCardsUseCase>();
		GetCardsResponse response = useCase.Execute(GetCardsRequest{this->deckId});
		for (const CardResponse& card : response.cards) {
			this->AddCard(card.cardId, wxString(card.front), wxString(card.back), wxString(card.tags));
		}
	}

	this->scroller->FitInside();
	this->scroller->Layout();

	if (previouslySelected != 0 && this->FindCard(previouslySelected) != nullptr) {
		this->SelectCard(previouslySelected);
	} else {
		this->selectedCardId = 0;
	}

	this->CallAfter([this]() {
		for (FlashCard* card : this->cards) {
			card->RelayoutContents();
		}
		this->scroller->Layout();
		this->scroller->FitInside();
	});
}

void FlashCardList::AddCard(int cardId, const wxString& front, const wxString& back, const wxString& tags) {
	FlashCard* card = new FlashCard(this->scroller, front, back, tags);
	card->SetCursor(wxCURSOR_HAND);
	const int topGap = this->cards.empty() ? 0 : this->FromDIP(8);
	this->listSizer->Add(card, 0, wxEXPAND | wxTOP, topGap);
	this->cards.push_back(card);
	this->cardIds.push_back(cardId);
	this->BindClicks(card, cardId);
}

FlashCard* FlashCardList::FindCard(int cardId) const {
	for (size_t i = 0; i < this->cardIds.size(); ++i) {
		if (this->cardIds[i] == cardId) {
			return this->cards[i];
		}
	}
	return nullptr;
}

void FlashCardList::BindClicks(wxWindow* window, int cardId) {
	window->Bind(wxEVT_LEFT_DOWN, [this, cardId](wxMouseEvent&) {
		this->SelectCard(cardId);
	});
	for (wxWindow* child : window->GetChildren()) {
		this->BindClicks(child, cardId);
	}
}

int FlashCardList::GetSelectedCardId() const {
	return this->selectedCardId;
}

void FlashCardList::SelectCard(int cardId) {
	this->selectedCardId = cardId;
	this->RefreshSelection();
}

void FlashCardList::RefreshSelection() {
	for (size_t i = 0; i < this->cards.size(); ++i) {
		this->cards[i]->SetSelected(this->cardIds[i] == this->selectedCardId);
	}
}

void FlashCardList::OnAdd(wxCommandEvent&) {
	if (this->deckId == 0) {
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

	CreateCardRequest request;
	request.deckId = this->deckId;
	request.front = front;
	request.back = back;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<CreateCardUseCase>();
	CreateCardResponse response = useCase.Execute(request);
	if (response.cardId == 0) {
		ShowCenteredMessage(this, "Could not save card.", "Add Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::OnEdit(wxCommandEvent&) {
	const int cardId = this->GetSelectedCardId();
	if (cardId == 0) {
		ShowCenteredMessage(this, "No card selected.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	FlashCard* card = this->FindCard(cardId);
	if (card == nullptr) { return; }

	CardDialog dialog(this, "Edit Card");
	dialog.frontCtrl->SetValue(card->GetFront());
	dialog.backCtrl->SetValue(card->GetBack());
	dialog.tagCtrl->SetValue(card->GetTags());
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string newFront = dialog.frontCtrl->GetValue().ToStdString();
	const std::string newBack = dialog.backCtrl->GetValue().ToStdString();
	if (newFront.empty() || newBack.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	UpdateCardRequest request;
	request.deckId = this->deckId;
	request.cardId = cardId;
	request.front = newFront;
	request.back = newBack;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<UpdateCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not update card.", "Edit Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::OnDelete(wxCommandEvent&) {
	const int cardId = this->GetSelectedCardId();
	if (cardId == 0) {
		ShowCenteredMessage(this, "No card selected.", "Delete Card", wxOK | wxICON_WARNING);
		return;
	}
	if (this->deckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Delete Card", wxOK | wxICON_WARNING);
		return;
	}

	const int confirmed = ShowCenteredMessage(
		this,
		"Delete this card?",
		"Delete Card",
		wxYES_NO | wxICON_QUESTION
	);
	if (confirmed != wxID_YES) { return; }

	DeleteCardRequest request;
	request.deckId = this->deckId;
	request.cardId = cardId;

	auto useCase = wxGetApp().GetInjector().create<DeleteCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not delete card.", "Delete Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::OnStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowStudyDeck();
	}
}
