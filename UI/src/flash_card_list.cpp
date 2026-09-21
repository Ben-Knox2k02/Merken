#include "flash_card_list.h"
#include "card_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"

wxDECLARE_APP(App);

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  selectedCardId(0),
	  lastHeaderWrap(0) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "");
	wxFont titleFont = this->header->GetFont();
	titleFont.MakeBold();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	this->header->SetFont(titleFont);
	this->header->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->header->SetMinSize(wxSize(0, -1));

	this->description = new wxStaticText(this, wxID_ANY, "");
	this->description->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	this->description->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->description->SetMinSize(wxSize(0, -1));
	this->description->Hide();

	wxBoxSizer* titleBlock = new wxBoxSizer(wxVERTICAL);
	titleBlock->Add(this->header, 0, wxEXPAND);
	titleBlock->Add(this->description, 0, wxEXPAND | wxTOP, 4);
	this->rootSizer->Add(titleBlock, 0, wxEXPAND | wxALL, 10);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	const int gap = this->FromDIP(8);
	this->listSizer = new wxFlexGridSizer(2, gap, gap);
	this->listSizer->AddGrowableCol(0, 1);
	this->listSizer->AddGrowableCol(1, 1);
	this->listSizer->SetFlexibleDirection(wxHORIZONTAL);
	this->listSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
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
	const int previousWrap = this->lastHeaderWrap;
	this->WrapHeader();
	if (this->lastHeaderWrap != previousWrap) {
		this->Layout();
	}
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

	this->UpdateDeckHeader();

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
		this->lastHeaderWrap = 0;
		this->WrapHeader();
		this->Layout();
		for (FlashCard* card : this->cards) {
			card->RelayoutContents();
		}
		this->scroller->Layout();
		this->scroller->FitInside();
	});
}

void FlashCardList::UpdateDeckHeader() {
	this->deckName = wxEmptyString;
	this->deckDescription = wxEmptyString;

	if (this->deckId != 0) {
		auto useCase = wxGetApp().GetInjector().create<GetDecksUseCase>();
		GetDecksResponse response = useCase.Execute();
		for (const DeckResponse& deck : response.decks) {
			if (deck.deckId == this->deckId) {
				this->deckName = wxString(deck.name);
				this->deckDescription = wxString(deck.description);
				this->deckDescription.Trim(true).Trim(false);
				break;
			}
		}
	}

	this->header->SetLabel(this->deckName);
	this->header->Show(!this->deckName.IsEmpty());
	this->description->SetLabel(this->deckDescription);
	this->description->Show(!this->deckDescription.IsEmpty());
	this->lastHeaderWrap = 0;
	this->WrapHeader();
	this->Layout();
}

void FlashCardList::WrapHeader() {
	const int wrapWidth = this->GetClientSize().GetWidth() - 2 * kMargin - 20;
	if (wrapWidth < this->FromDIP(40)) {
		return;
	}
	if (this->lastHeaderWrap != 0 &&
		wrapWidth >= this->lastHeaderWrap - this->FromDIP(8) &&
		wrapWidth <= this->lastHeaderWrap + this->FromDIP(8)) {
		return;
	}
	this->lastHeaderWrap = wrapWidth;
	this->header->SetLabel(this->deckName);
	this->header->Wrap(wrapWidth);
	if (this->description->IsShown()) {
		this->description->SetLabel(this->deckDescription);
		this->description->Wrap(wrapWidth);
	}
}

void FlashCardList::AddCard(int cardId, const wxString& front, const wxString& back, const wxString& tags) {
	FlashCard* card = new FlashCard(this->scroller, front, back, tags);
	card->SetCursor(wxCURSOR_HAND);
	// Unwrapped text would otherwise set a large min width and make the columns unequal.
	card->SetMinSize(wxSize(0, -1));
	this->listSizer->Add(card, 0, wxEXPAND);
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
