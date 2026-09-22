#include "flash_card_list.h"
#include "card_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "theme.h"
#include "guide_highlight.h"
#include "app.h"
#include <wx/dcgraph.h>
#include <wx/tokenzr.h>
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"

wxDECLARE_APP(App);

namespace {

wxString WrapToWidth(wxWindow* win, const wxString& text, int width) {
	wxString result;
	wxStringTokenizer paragraphs(text, "\n", wxTOKEN_RET_EMPTY);
	bool firstParagraph = true;
	while (paragraphs.HasMoreTokens()) {
		const wxString paragraph = paragraphs.GetNextToken();
		wxString line;
		wxStringTokenizer words(paragraph, " \t");
		while (words.HasMoreTokens()) {
			const wxString word = words.GetNextToken();
			if (word.empty()) {
				continue;
			}
			const wxString trial = line.empty() ? word : line + " " + word;
			if (!line.empty() && win->GetTextExtent(trial).GetWidth() > width) {
				if (!result.empty() || !firstParagraph) {
					result += "\n";
				}
				result += line;
				line = word;
				firstParagraph = false;
			} else {
				line = trial;
			}
		}
		if (!line.empty()) {
			if (!result.empty() || !firstParagraph) {
				result += "\n";
			}
			result += line;
			firstParagraph = false;
		}
	}
	return result.empty() ? text : result;
}

int LabelHeight(wxWindow* win, const wxString& wrapped) {
	const int lineH = win->GetTextExtent("Ag").GetHeight();
	if (wrapped.empty()) {
		return lineH;
	}
	int lines = 1;
	for (size_t i = 0; i < wrapped.length(); ++i) {
		if (wrapped[i] == '\n') {
			++lines;
		}
	}
	return lines * lineH;
}

}

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  lastHeaderWrap(0),
	  lastDescWrap(0) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(Theme::Get().color.window);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "");
	wxFont titleFont = this->header->GetFont();
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
	this->headerButtonSizer->Add(this->addButton, 0, wxRIGHT | wxALIGN_BOTTOM, Theme::Get().space.xs);
	this->headerButtonSizer->Add(this->studyButton, 0, wxRIGHT | wxALIGN_BOTTOM, Theme::Get().space.xs);
	this->headerButtonSizer->Add(this->aiStudyButton, 0);

	wxBoxSizer* headerRow = new wxBoxSizer(wxHORIZONTAL);
	headerRow->Add(this->header, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxRIGHT, Theme::Get().space.md);
	headerRow->Add(this->headerButtonSizer, 0, wxALIGN_TOP);

	wxBoxSizer* headerBlock = new wxBoxSizer(wxVERTICAL);
	headerBlock->Add(headerRow, 0, wxEXPAND);
	headerBlock->Add(this->description, 0, wxEXPAND | wxTOP | wxBOTTOM, Theme::Get().space.xxl);
	this->rootSizer->Add(headerBlock, 0, wxEXPAND | wxALL, pad);

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

	this->emptyPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->emptyPanel->SetBackgroundColour(Theme::Get().color.window);
	this->emptyMessage = new wxStaticText(this->emptyPanel, wxID_ANY, "No flash cards yet");
	this->emptyMessage->SetForegroundColour(Theme::Get().color.secondaryLabel);
	wxBoxSizer* emptySizer = new wxBoxSizer(wxVERTICAL);
	emptySizer->AddStretchSpacer(1);
	emptySizer->Add(this->emptyMessage, 0, wxALIGN_CENTER_HORIZONTAL);
	emptySizer->AddStretchSpacer(1);
	this->emptyPanel->SetSizer(emptySizer);

	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
	this->rootSizer->Add(this->emptyPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
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
	const int previousTitle = this->lastHeaderWrap;
	const int previousDesc = this->lastDescWrap;
	this->WrapHeader();
	if (this->lastHeaderWrap != previousTitle || this->lastDescWrap != previousDesc) {
		this->Layout();
	}
	this->scroller->Layout();
	this->scroller->FitInside();
	this->Refresh();
	event.Skip();
}

void FlashCardList::ApplyTheme() {
	const Theme& theme = Theme::Get();
	const bool invert = Theme::IsDarkAppearance();
	this->SetBackgroundColour(theme.color.window);
	this->header->SetForegroundColour(theme.color.label);
	this->description->SetForegroundColour(theme.color.secondaryLabel);
	this->scroller->SetBackgroundColour(theme.color.window);
	if (this->emptyPanel != nullptr) {
		this->emptyPanel->SetBackgroundColour(theme.color.window);
	}
	if (this->emptyMessage != nullptr) {
		this->emptyMessage->SetForegroundColour(theme.color.secondaryLabel);
	}
	this->addButton->SetBitmap(IconButton::LoadIconBundle("UI/assets/add_icon.png", 16, invert), wxLEFT);
	this->studyButton->SetBitmap(IconButton::LoadIconBundle("UI/assets/study_icon.png", 16, invert), wxLEFT);
	this->aiStudyButton->SetBitmap(IconButton::LoadIconBundle("UI/assets/ai_study_icon.png", 16, invert), wxLEFT);
	for (FlashCard* card : this->cards) {
		card->ApplyTheme();
	}
	this->RefreshGuide();
	this->Refresh();
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
	this->ShowEmptyState(this->cards.empty());
	this->UpdateStudyButtons();
	this->RefreshGuide();

	this->CallAfter([this]() {
		this->lastHeaderWrap = 0;
		this->lastDescWrap = 0;
		this->WrapHeader();
		this->Layout();
		for (FlashCard* card : this->cards) {
			card->RelayoutContents();
		}
		this->scroller->Layout();
		this->scroller->FitInside();
	});
}

void FlashCardList::RefreshGuide() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = useCase.Execute();
	const bool guided = profile.ok && !profile.guideFinished && this->deckId != 0;
	if (guided && this->cards.empty()) {
		GuideHighlight::SetBorder(this->studyButton, false);
		GuideHighlight::Announce(
			this->addButton,
			GuidePrompt::AddCard,
			"Let's add a flash-card to the deck. Write the front and the back, and add any tags that help you remember."
		);
		return;
	}
	if (guided) {
		GuideHighlight::SetBorder(this->addButton, false);
		GuideHighlight::Announce(
			this->studyButton,
			GuidePrompt::StudyDeck,
			"Let's study it together. The front comes up first, and then you can reveal the back."
		);
		return;
	}
	GuideHighlight::SetBorder(this->addButton, false);
	GuideHighlight::SetBorder(this->studyButton, false);
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
	this->lastDescWrap = 0;
	this->WrapHeader();
	this->Layout();
}

void FlashCardList::ShowEmptyState(bool empty) {
	if (this->rootSizer != nullptr) {
		this->rootSizer->Show(this->scroller, !empty);
		this->rootSizer->Show(this->emptyPanel, empty);
	}
	this->Layout();
}

void FlashCardList::UpdateStudyButtons() {
	const bool canStudy = this->deckId != 0 && !this->cards.empty();
	this->studyButton->Enable(canStudy);
	this->aiStudyButton->Enable(canStudy);
}

void FlashCardList::WrapHeader() {
	const int pad = Theme::Get().size.panelPad;
	const int minWrap = this->FromDIP(Theme::Get().size.minWrap);
	const int buttons = this->headerButtonSizer != nullptr ? this->headerButtonSizer->GetMinSize().GetWidth() : 0;

	int titleWrap = this->header->GetClientSize().GetWidth();
	if (titleWrap < minWrap) {
		titleWrap = this->GetClientSize().GetWidth() - 2 * pad - buttons - Theme::Get().space.md;
	}
	if (titleWrap >= minWrap && this->lastHeaderWrap != titleWrap) {
		this->lastHeaderWrap = titleWrap;
		const wxString title = WrapToWidth(this->header, this->deckName, titleWrap);
		this->header->SetLabel(title);
		this->header->SetMinSize(wxSize(0, LabelHeight(this->header, title)));
	}

	if (!this->description->IsShown()) {
		return;
	}
	int descWrap = this->description->GetClientSize().GetWidth();
	if (descWrap < minWrap) {
		descWrap = this->GetClientSize().GetWidth() - 2 * pad;
	}
	if (descWrap < minWrap || this->lastDescWrap == descWrap) {
		return;
	}
	this->lastDescWrap = descWrap;
	const wxString desc = WrapToWidth(this->description, this->deckDescription, descWrap);
	this->description->SetLabel(desc);
	this->description->SetMinSize(wxSize(0, LabelHeight(this->description, desc)));
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
