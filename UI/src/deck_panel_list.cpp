#include "deck_panel_list.h"
#include "deck_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateDeck/create_deck_usecase.h"
#include "../../Application/UseCases/Deck/UpdateDeck/update_deck_usecase.h"

wxDECLARE_APP(App);

DeckPanelList::DeckPanelList(wxWindow* parent)
	: wxPanel(parent),
	  selectedDeckId(0) {
	this->SetMinSize(wxSize(320, -1));

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Decks");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxALL, 10);

	this->addDeckButton = new wxButton(this, wxID_ANY, "Add");
	this->editDeckButton = new wxButton(this, wxID_ANY, "Edit");
	this->deleteDeckButton = new wxButton(this, wxID_ANY, "Delete");

	this->buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->buttonSizer->Add(this->addDeckButton, 1, wxRIGHT, 5);
	this->buttonSizer->Add(this->editDeckButton, 1, wxRIGHT, 5);
	this->buttonSizer->Add(this->deleteDeckButton, 1);
	this->rootSizer->Add(this->buttonSizer, 0, wxEXPAND | wxALL, 10);

	this->SetSizer(this->rootSizer);

	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnAddDeck, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnEditDeck, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnDeleteDeck, this);

	this->LoadDecks();
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
		this->AddDeckCard(deck.deckId, wxString(deck.name), wxString(deck.description));
		if (firstDeckId == 0) {
			firstDeckId = deck.deckId;
		}
	}

	this->scroller->FitInside();
	this->scroller->Layout();

	if (previouslySelected != 0) {
		this->SelectDeck(previouslySelected, false);
	} else if (firstDeckId != 0) {
		this->SelectDeck(firstDeckId, false);
	} else {
		this->selectedDeckId = 0;
	}
}

void DeckPanelList::AddDeckCard(int deckId, const wxString& name, const wxString& description) {
	DeckCardIcon* card = new DeckCardIcon(this->scroller, name, description);
	card->SetCursor(wxCURSOR_HAND);
	this->listSizer->Add(card, 0, wxEXPAND | wxBOTTOM, 4);
	this->cards.push_back(card);
	this->cardIds.push_back(deckId);
	this->BindClicks(card, deckId);
}

DeckCardIcon* DeckPanelList::FindCard(int deckId) const {
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
	if (deckId == 0) { return; }
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
	this->SelectDeck(deckId, false);
}

void DeckPanelList::OnDeleteDeck(wxCommandEvent&) {
	ShowCenteredMessage(this, "Delete is not implemented yet.", "Delete Deck", wxOK | wxICON_INFORMATION);
}
