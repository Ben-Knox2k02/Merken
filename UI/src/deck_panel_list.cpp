#include "deck_panel_list.h"
#include "dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "theme.h"
#include "guide_highlight.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateDeck/create_deck_usecase.h"
#include "../../Application/UseCases/Deck/UpdateDeck/update_deck_usecase.h"
#include "../../Application/UseCases/Deck/DeleteDeck/delete_deck_usecase.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"
#include <wx/dcgraph.h>

wxDECLARE_APP(App);

DeckPanelList::DeckPanelList(wxWindow* parent)
	: wxPanel(),
	  selectedDeckId(0) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->SetBackgroundColour(Theme::Get().color.window);
	this->SetMinSize(wxSize(this->FromDIP(kMinWidth), -1));

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const int pad = this->FromDIP(Theme::Get().size.cardPad);
	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().size.panelPad));
	this->profileHeader = new ProfileHeader(this);
	this->rootSizer->Add(this->profileHeader, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->decksHeading = new wxStaticText(this, wxID_ANY, "My Decks");
	wxFont headingFont = this->decksHeading->GetFont();
	headingFont.SetWeight(wxFONTWEIGHT_NORMAL);
	this->decksHeading->SetFont(headingFont);
	this->decksHeading->SetForegroundColour(Theme::Get().color.secondaryLabel);
	this->rootSizer->Add(this->decksHeading, 0, wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, pad);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->scroller->SetWindowStyleFlag(this->scroller->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->scroller->SetBackgroundColour(Theme::Get().color.window);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT, pad);

	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().space.md));

	this->addDeckButton = new wxButton(this, wxID_ANY, "Add");
	this->editDeckButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteDeckButton = new wxButton(this, wxID_ANY, "Delete");
	ApplyButtonPadding(this->addDeckButton);
	ApplyButtonPadding(this->editDeckButton);
	ApplyButtonPadding(this->deleteDeckButton);

	const int gap = this->FromDIP(Theme::Get().space.xs);
	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->buttonSizer->AddStretchSpacer(1);
	this->buttonSizer->Add(this->addDeckButton, 0, wxRIGHT, gap);
	this->buttonSizer->Add(this->editDeckButton, 0, wxRIGHT, gap);
	this->buttonSizer->Add(this->deleteDeckButton, 0);
	this->buttonSizer->AddStretchSpacer(1);
	this->rootSizer->Add(this->buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

	this->SetSizer(this->rootSizer);

	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnAddDeck, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnEditDeck, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnDeleteDeck, this);
	this->Bind(wxEVT_PAINT, &DeckPanelList::OnPaint, this);
	this->Bind(wxEVT_SIZE, &DeckPanelList::OnSize, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->LoadDecks();
}

void DeckPanelList::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	this->profileHeader->ApplyTheme();
	this->decksHeading->SetForegroundColour(theme.color.secondaryLabel);
	this->scroller->SetBackgroundColour(theme.color.window);
	for (DeckCard* card : this->cards) {
		card->ApplyTheme();
	}
	this->RefreshGuide();
	this->Refresh();
}

void DeckPanelList::OnSize(wxSizeEvent& event) {
	this->Refresh();
	event.Skip();
}

void DeckPanelList::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	wxGCDC gc(dc);
	Theme::Get().FillCanvas(gc, this, this->GetClientSize());
}

void DeckPanelList::LoadDecks() {
	const int previouslySelected = this->GetSelectedDeckId();
	this->listSizer->Clear(true);
	this->cards.clear();
	this->cardIds.clear();

	auto useCase = wxGetApp().GetInjector().create<GetDecksUseCase>();
	GetDecksResponse response = useCase.Execute();

	int firstDeckId = 0;
	for (const DeckResponse& deck : response.decks) {
		this->AddDeckCard(deck.deckId, wxString(deck.name), deck.cardCount);
		if (firstDeckId == 0) {
			firstDeckId = deck.deckId;
		}
	}

	this->scroller->FitInside();
	this->scroller->Layout();

	if (previouslySelected != 0 && this->FindCard(previouslySelected) != nullptr) {
		this->SelectDeck(previouslySelected, false);
	} else if (firstDeckId != 0) {
		this->SelectDeck(firstDeckId, true);
	} else {
		this->selectedDeckId = 0;
		this->NotifyDeckSelected(0);
	}
	this->RefreshGuide();
}

void DeckPanelList::RefreshGuide() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = useCase.Execute();
	const bool guiding = profile.ok && !profile.guideFinished;
	const bool hasDecks = !this->cards.empty();
	const bool createDeck = guiding && !hasDecks;
	if (!guiding) {
		this->Enable(true);
	}
	this->addDeckButton->Enable(!guiding || createDeck);
	this->editDeckButton->Enable(!guiding && hasDecks);
	this->deleteDeckButton->Enable(!guiding && hasDecks);
	this->profileHeader->Enable(!guiding);
	for (DeckCard* card : this->cards) {
		card->Enable(!guiding);
	}
	if (createDeck) {
		GuideHighlight::Announce(this->addDeckButton, GuidePrompt::CreateDeck, "Let's create your first flash-card deck.");
	} else {
		GuideHighlight::SetBorder(this->addDeckButton, false);
	}
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->UpdateDeckMenus();
	}
}

void DeckPanelList::AddDeckCard(int deckId, const wxString& name, int cardCount) {
	DeckCard* card = new DeckCard(this->scroller, name, cardCount);
	card->SetCursor(wxCURSOR_HAND);
	this->listSizer->Add(card, 0, wxEXPAND);
	this->cards.push_back(card);
	this->cardIds.push_back(deckId);
	this->BindClicks(card, deckId);
}

DeckCard* DeckPanelList::FindCard(int deckId) const {
	for (size_t i = 0; i < this->cardIds.size(); ++i) {
		if (this->cardIds[i] == deckId) {
			return this->cards[i];
		}
	}
	return nullptr;
}

void DeckPanelList::BindClicks(wxWindow* window, int deckId) {
	window->Bind(wxEVT_LEFT_DOWN, [this, deckId](wxMouseEvent&) {
		this->SelectDeck(deckId, true);
	});
	for (wxWindow* child : window->GetChildren()) {
		this->BindClicks(child, deckId);
	}
}

int DeckPanelList::GetSelectedDeckId() const {
	return this->selectedDeckId;
}

void DeckPanelList::SelectDeck(int deckId, bool notify) {
	this->selectedDeckId = deckId;
	this->RefreshSelection();
	if (notify) {
		this->NotifyDeckSelected(deckId);
	}
}

void DeckPanelList::RefreshSelection() {
	for (size_t i = 0; i < this->cards.size(); ++i) {
		this->cards[i]->SetSelected(this->cardIds[i] == this->selectedDeckId);
	}
}

void DeckPanelList::NotifyDeckSelected(int deckId) {
	if (auto* frame = dynamic_cast<MainFrame*>(this->GetParent())) {
		frame->OnDeckSelected(deckId);
	}
}

void DeckPanelList::OnAddDeck(wxCommandEvent&) {
	DeckDialog dialog(this, "Add Deck");
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string name = dialog.nameCtrl->GetValue().ToStdString();
	if (name.empty()) {
		ShowCenteredMessage(this, "Deck name is required.", "Add Deck", wxOK | wxICON_WARNING);
		return;
	}

	CreateDeckRequest request;
	request.name = name;
	request.description = dialog.descriptionCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<CreateDeckUseCase>();
	CreateDeckResponse response = useCase.Execute(request);
	if (response.deckId == 0) {
		ShowCenteredMessage(this, "Could not save deck.", "Add Deck", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadDecks();
	this->SelectDeck(response.deckId, true);
}

void DeckPanelList::OnEditDeck(wxCommandEvent&) {
	const int deckId = this->GetSelectedDeckId();
	if (deckId == 0) {
		ShowCenteredMessage(this, "No deck selected.", "Edit Deck", wxOK | wxICON_WARNING);
		return;
	}

	auto getDecks = wxGetApp().GetInjector().create<GetDecksUseCase>();
	GetDecksResponse decks = getDecks.Execute();
	const DeckResponse* selected = nullptr;
	for (const DeckResponse& deck : decks.decks) {
		if (deck.deckId == deckId) {
			selected = &deck;
			break;
		}
	}
	if (selected == nullptr) { return; }

	DeckDialog dialog(this, "Edit Deck");
	dialog.nameCtrl->SetValue(wxString(selected->name));
	dialog.descriptionCtrl->SetValue(wxString(selected->description));
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string name = dialog.nameCtrl->GetValue().ToStdString();
	if (name.empty()) {
		ShowCenteredMessage(this, "Deck name is required.", "Edit Deck", wxOK | wxICON_WARNING);
		return;
	}

	UpdateDeckRequest request;
	request.deckId = deckId;
	request.name = name;
	request.description = dialog.descriptionCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<UpdateDeckUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not update deck.", "Edit Deck", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadDecks();
	this->SelectDeck(deckId, true);
}

void DeckPanelList::OnDeleteDeck(wxCommandEvent&) {
	const int deckId = this->GetSelectedDeckId();
	if (deckId == 0) {
		ShowCenteredMessage(this, "No deck selected.", "Delete Deck", wxOK | wxICON_WARNING);
		return;
	}

	wxString name = "this deck";
	if (DeckCard* card = this->FindCard(deckId)) {
		name = card->GetTitle();
	}
	const int confirmed = ShowCenteredMessage(
		this,
		"Delete \"" + name + "\" and all of its cards?",
		"Delete Deck",
		wxYES_NO | wxICON_QUESTION,
		nullptr,
		nullptr,
		true
	);
	if (confirmed != wxID_YES) { return; }

	DeleteDeckRequest request;
	request.deckId = deckId;
	auto useCase = wxGetApp().GetInjector().create<DeleteDeckUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not delete deck.", "Delete Deck", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadDecks();
}
