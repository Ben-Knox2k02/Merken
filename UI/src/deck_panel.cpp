#include "deck_panel.h"
#include "deck_dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateDeck/create_deck_usecase.h"
#include "../../Application/UseCases/Deck/UpdateDeck/update_deck_usecase.h"

wxDECLARE_APP(App);

DeckPanel::DeckPanel(wxWindow* parent) : wxPanel(parent) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->SetMinSize(wxSize(240, -1));

	this->header = new wxStaticText(this, wxID_ANY, "Decks");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->deckList = new wxDataViewCtrl(this, wxID_ANY);
	this->deckList->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);
	this->deckList->AppendTextColumn("Description", 1, wxDATAVIEW_CELL_INERT, 200, wxALIGN_CENTER);

	this->deckViewModel = new wxDataViewListStore();
	this->deckList->AssociateModel(this->deckViewModel);
	this->deckViewModel->DecRef();

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

	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnAddDeck, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnEditDeck, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanel::OnDeleteDeck, this);
	this->deckList->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &DeckPanel::OnDeckActivated, this);
	this->deckList->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &DeckPanel::OnDeckSelectionChanged, this);
}

void DeckPanel::LoadDecks() {
	const int previouslySelected = this->GetSelectedDeckId();
	this->deckViewModel->DeleteAllItems();

	auto useCase = wxGetApp().GetInjector().create<GetDecksUseCase>();
	GetDecksResponse response = useCase.Execute();

	int firstDeckId = 0;
	for (const DeckResponse& deck : response.decks) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(wxString(deck.name)));
		row.push_back(wxVariant(wxString(deck.description)));
		this->deckViewModel->AppendItem(row, static_cast<wxUIntPtr>(deck.deckId));
		if (firstDeckId == 0) { firstDeckId = deck.deckId; }
	}

	if (previouslySelected != 0) {
		this->SelectDeck(previouslySelected);
	} else if (firstDeckId != 0) {
		this->SelectDeck(firstDeckId);
		this->NotifyDeckSelected(firstDeckId);
	}
}

int DeckPanel::GetSelectedDeckId() const {
	wxDataViewItem item = this->deckList->GetSelection();
	if (!item.IsOk()) { return 0; }
	return static_cast<int>(this->deckViewModel->GetItemData(item));
}

void DeckPanel::SelectDeck(int deckId) {
	const unsigned int count = this->deckViewModel->GetCount();
	for (unsigned int row = 0; row < count; ++row) {
		wxDataViewItem item = this->deckViewModel->GetItem(row);
		if (static_cast<int>(this->deckViewModel->GetItemData(item)) == deckId) {
			this->deckList->Select(item);
			return;
		}
	}
}

void DeckPanel::NotifyDeckSelected(int deckId) {
	if (deckId == 0) { return; }
	if (auto* frame = dynamic_cast<MainFrame*>(this->GetParent())) {
		frame->OnDeckSelected(deckId);
	}
}

void DeckPanel::OnAddDeck(wxCommandEvent&) {
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
	this->SelectDeck(response.deckId);
	this->NotifyDeckSelected(response.deckId);
}

void DeckPanel::OnEditDeck(wxCommandEvent&) {
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
	this->SelectDeck(deckId);
}

void DeckPanel::OnDeleteDeck(wxCommandEvent&) {
	ShowCenteredMessage(this, "Delete is not implemented yet.", "Delete Deck", wxOK | wxICON_INFORMATION);
}

void DeckPanel::OnDeckActivated(wxDataViewEvent& event) {
	if (!event.GetItem().IsOk()) { return; }
	this->NotifyDeckSelected(static_cast<int>(this->deckViewModel->GetItemData(event.GetItem())));
}

void DeckPanel::OnDeckSelectionChanged(wxDataViewEvent& event) {
	if (!event.GetItem().IsOk()) { return; }
	this->NotifyDeckSelected(static_cast<int>(this->deckViewModel->GetItemData(event.GetItem())));
}
