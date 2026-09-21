#include "flash_card_list.h"
#include "card_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "theme.h"
#include "app.h"
#include <wx/dcgraph.h>
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"

wxDECLARE_APP(App);

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  lastHeaderWrap(0) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(Theme::Get().color.window);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "");
	wxFont titleFont = this->header->GetFont();
	titleFont.MakeBold();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	this->header->SetFont(titleFont);
	this->header->SetForegroundColour(Theme::Get().color.label);
	this->header->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->header->SetMinSize(wxSize(0, -1));

	this->description = new wxStaticText(this, wxID_ANY, "");
	this->description->SetForegroundColour(Theme::Get().color.secondaryLabel);
	this->description->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->description->SetMinSize(wxSize(0, -1));
	this->description->Hide();

	const int pad = Theme::Get().size.panelPad;
	wxBoxSizer* titleBlock = new wxBoxSizer(wxVERTICAL);
	titleBlock->Add(this->header, 0, wxEXPAND);
	titleBlock->Add(this->description, 0, wxEXPAND | wxTOP, Theme::Get().space.xs);

	this->headerButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->addButton = new wxButton(this, wxID_ANY, "Add Flash Card");
	this->addButton->SetBitmap(
		IconButton::LoadIconBundle("UI/assets/add_icon.png", 16, Theme::IsDarkAppearance()),
		wxLEFT
	);
	this->addButton->SetBitmapMargins(this->FromDIP(6), 0);
	this->studyButton = new wxButton(this, wxID_ANY, "Study Deck");
	this->studyButton->SetBitmap(
		IconButton::LoadIconBundle("UI/assets/study_icon.png", 16, Theme::IsDarkAppearance()),
		wxLEFT
	);
	this->studyButton->SetBitmapMargins(this->FromDIP(6), 0);
	this->aiStudyButton = new wxButton(this, wxID_ANY, "AI Study Deck");
	this->aiStudyButton->SetBitmap(
		IconButton::LoadIconBundle("UI/assets/ai_study_icon.png", 16, Theme::IsDarkAppearance()),
		wxLEFT
	);
	this->aiStudyButton->SetBitmapMargins(this->FromDIP(6), 0);
	this->headerButtonSizer->Add(this->addButton, 0, wxRIGHT, Theme::Get().space.xs);
	this->headerButtonSizer->Add(this->studyButton, 0, wxRIGHT, Theme::Get().space.xs);
	this->headerButtonSizer->Add(this->aiStudyButton, 0);

	wxBoxSizer* headerRow = new wxBoxSizer(wxHORIZONTAL);
	headerRow->Add(titleBlock, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, Theme::Get().space.md);
	headerRow->Add(this->headerButtonSizer, 0, wxALIGN_CENTER_VERTICAL);
	this->rootSizer->Add(headerRow, 0, wxEXPAND | wxALL, pad);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
	this->scroller->SetBackgroundColour(Theme::Get().color.window);

	const int gap = this->FromDIP(Theme::Get().size.listGap);
	this->leftCol = new wxBoxSizer(wxVERTICAL);
	this->rightCol = new wxBoxSizer(wxVERTICAL);
	this->listSizer = new wxBoxSizer(wxHORIZONTAL);
	this->listSizer->Add(this->leftCol, 1, wxEXPAND | wxRIGHT, gap / 2);
	this->listSizer->Add(this->rightCol, 1, wxEXPAND | wxLEFT, gap / 2);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
	this->SetSizer(this->rootSizer);

	this->addButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAdd, this);
	this->studyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnStudy, this);
	this->aiStudyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAiStudy, this);
	this->Bind(wxEVT_PAINT, &FlashCardList::OnPaint, this);
	this->Bind(wxEVT_SIZE, &FlashCardList::OnSize, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->LoadCards();
}

void FlashCardList::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	wxGCDC gc(dc);
	Theme::Get().FillCanvas(gc, this, this->GetClientSize());
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
	this->leftCol->Clear(true);
	this->rightCol->Clear(true);
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
	this->UpdateStudyButtons();

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

void FlashCardList::UpdateStudyButtons() {
	const bool canStudy = this->deckId != 0 && !this->cards.empty();
	this->studyButton->Enable(canStudy);
	this->aiStudyButton->Enable(canStudy);
}

void FlashCardList::WrapHeader() {
	const int pad = Theme::Get().size.panelPad;
	const int buttons = this->headerButtonSizer != nullptr ? this->headerButtonSizer->GetMinSize().GetWidth() : 0;
	const int wrapWidth = this->GetClientSize().GetWidth() - 2 * pad - buttons - Theme::Get().space.md;
	if (wrapWidth < this->FromDIP(Theme::Get().size.minWrap)) {
		return;
	}
	if (this->lastHeaderWrap != 0 &&
		wrapWidth >= this->lastHeaderWrap - this->FromDIP(Theme::Get().size.wrapDeadband) &&
		wrapWidth <= this->lastHeaderWrap + this->FromDIP(Theme::Get().size.wrapDeadband)) {
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
	const int number = static_cast<int>(this->cards.size()) + 1;
	FlashCard* card = new FlashCard(this->scroller, number, front, back, tags);
	// Unwrapped text would otherwise set a large min width and make the columns unequal.
	card->SetMinSize(wxSize(0, -1));
	card->SetOnEdit([this, cardId]() {
		this->EditCard(cardId);
	});
	card->SetOnDelete([this, cardId]() {
		this->DeleteCard(cardId);
	});
	const int gap = this->FromDIP(Theme::Get().size.listGap);
	wxBoxSizer* column = this->cards.size() % 2 == 0 ? this->leftCol : this->rightCol;
	column->Add(card, 0, wxEXPAND | wxBOTTOM, gap);
	this->cards.push_back(card);
	this->cardIds.push_back(cardId);
}

FlashCard* FlashCardList::FindCard(int cardId) const {
	for (size_t i = 0; i < this->cardIds.size(); ++i) {
		if (this->cardIds[i] == cardId) {
			return this->cards[i];
		}
	}
	return nullptr;
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ReloadDecks();
	}
}

void FlashCardList::EditCard(int cardId) {
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

void FlashCardList::DeleteCard(int cardId) {
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ReloadDecks();
	}
}

void FlashCardList::OnStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowStudyDeck();
	}
}

void FlashCardList::OnAiStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowAiStudy();
	}
}
