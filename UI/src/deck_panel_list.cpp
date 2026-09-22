#include "deck_panel_list.h"
#include "deck_dialog.h"
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
#include <wx/statline.h>

wxDECLARE_APP(App);

DeckPanelList::DeckPanelList(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
	  selectedDeckId(0) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(Theme::Get().color.primary);
	this->SetMinSize(wxSize(kMinWidth, -1));

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const int pad = Theme::Get().size.cardPad;
	const int profileSpace = Theme::Get().space.xl;
	this->rootSizer->AddSpacer(profileSpace);
	this->profileHeader = new ProfileHeader(this);
	this->rootSizer->Add(this->profileHeader, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->rootSizer->AddSpacer(profileSpace);
	wxStaticLine* profileRule = new wxStaticLine(this);
	this->rootSizer->Add(profileRule, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->rootSizer->AddSpacer(profileSpace);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
	this->scroller->SetBackgroundColour(Theme::Get().color.primary);

	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT, pad);

	wxStaticLine* rule = new wxStaticLine(this);
	this->rootSizer->Add(rule, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->rootSizer->AddSpacer(Theme::Get().space.md);

	this->addDeckButton = new IconButton(this, "UI/assets/add_icon.png", "Add", true);
	this->editDeckButton = new IconButton(this, "UI/assets/edit_icon.png", "Edit", true);
	this->deleteDeckButton = new IconButton(this, "UI/assets/delete_icon.png", "Delete", true);

	this->buttonSizer = new wxFlexGridSizer(1, 3, 0, Theme::Get().space.xs);
	this->buttonSizer->AddGrowableCol(0, 1);
	this->buttonSizer->AddGrowableCol(1, 1);
	this->buttonSizer->AddGrowableCol(2, 1);
	this->buttonSizer->SetFlexibleDirection(wxHORIZONTAL);
	this->buttonSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
	this->buttonSizer->Add(this->addDeckButton, 1, wxEXPAND);
	this->buttonSizer->Add(this->editDeckButton, 1, wxEXPAND);
	this->buttonSizer->Add(this->deleteDeckButton, 1, wxEXPAND);
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
	this->SetBackgroundColour(theme.color.primary);
	this->profileHeader->ApplyTheme();
	this->scroller->SetBackgroundColour(theme.color.primary);
	this->addDeckButton->ApplyTheme();
	this->editDeckButton->ApplyTheme();
	this->deleteDeckButton->ApplyTheme();
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
	Theme::Get().DrawPanel(gc, this, this->GetClientSize());
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
	const bool showAdd = profile.ok && !profile.guideFinished && this->cards.empty();
	if (showAdd) {
		GuideHighlight::Announce(this->addDeckButton, GuidePrompt::CreateDeck, "Let's create your first flash-card deck.");
		return;
	}
	GuideHighlight::SetBorder(this->addDeckButton, false);
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
		wxYES_NO | wxICON_QUESTION
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
