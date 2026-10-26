#include "deck_home_panel.h"
#include "pastel_deck_card.h"
#include "mainframe.h"
#include "deck_panel_list.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"

#include <wx/statline.h>
#include <wx/dcbuffer.h>

wxDECLARE_APP(App);

namespace {
enum {
	ID_HOME_ADD = wxID_HIGHEST + 400,
	ID_HOME_GRID,
	ID_HOME_LIST,
	ID_HOME_SEARCH
};
} // namespace

wxBitmap DeckHomePanel::MakeToolBitmap(int kind, int size) {
	wxBitmap bmp(size, size, 32);
	wxMemoryDC dc(bmp);
	dc.SetBackground(wxBrush(wxColour(255, 255, 255)));
	dc.Clear();
	dc.SetPen(wxPen(wxColour(90, 90, 110), 2));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	const int m = 3;
	switch (kind) {
		case 0:
			dc.DrawLine(size / 2, m, size / 2, size - m);
			dc.DrawLine(m, size / 2, size - m, size / 2);
			break;
		case 1:
			dc.DrawRectangle(m, m, 5, 5);
			dc.DrawRectangle(size - m - 5, m, 5, 5);
			dc.DrawRectangle(m, size - m - 5, 5, 5);
			dc.DrawRectangle(size - m - 5, size - m - 5, 5, 5);
			break;
		case 2:
			dc.DrawLine(m, m + 2, size - m, m + 2);
			dc.DrawLine(m, size / 2, size - m, size / 2);
			dc.DrawLine(m, size - m - 2, size - m, size - m - 2);
			break;
		default:
			break;
	}
	dc.SelectObject(wxNullBitmap);
	return bmp;
}

DeckHomePanel::DeckHomePanel(wxWindow* parent)
	: wxPanel() {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->SetBackgroundColour(Theme::Get().color.window);

	auto* root = new wxBoxSizer(wxVERTICAL);
	this->BuildToolbar();
	root->Add(this->toolbar, 0, wxEXPAND | wxTOP | wxBOTTOM, this->FromDIP(8));
	root->Add(new wxStaticLine(this), 0, wxEXPAND);

	this->scroll = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxBORDER_NONE);
	this->scroll->SetBackgroundColour(Theme::Get().color.window);
	this->scroll->SetScrollRate(0, 16);

	this->cardsHost = new wxPanel(this->scroll);
	this->cardsHost->SetBackgroundColour(Theme::Get().color.window);
	this->wrap = new wxWrapSizer(wxHORIZONTAL);
	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->cardsHost->SetSizer(this->wrap);

	auto* scrollSizer = new wxBoxSizer(wxVERTICAL);
	scrollSizer->Add(this->cardsHost, 1, wxEXPAND | wxALL, this->FromDIP(16));
	this->scroll->SetSizer(scrollSizer);
	root->Add(this->scroll, 1, wxEXPAND);

	this->SetSizer(root);
	this->Bind(wxEVT_PAINT, &DeckHomePanel::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->LoadDecks();
}

void DeckHomePanel::BuildToolbar() {
	this->toolbar = new wxPanel(this);
	this->toolbar->SetBackgroundColour(Theme::Get().color.window);
	auto* tb = new wxBoxSizer(wxHORIZONTAL);

	this->titleLabel = new wxStaticText(this->toolbar, wxID_ANY, "My Decks");
	wxFont titleFont = this->titleLabel->GetFont();
	titleFont.SetWeight(wxFONTWEIGHT_BOLD);
	titleFont.SetPointSize(titleFont.GetPointSize() + 2);
	this->titleLabel->SetFont(titleFont);
	this->titleLabel->SetForegroundColour(Theme::Get().color.label);

	auto makeBtn = [this](int id, int kind, const wxString& tip) {
		auto* btn = new wxBitmapButton(
			this->toolbar,
			id,
			MakeToolBitmap(kind),
			wxDefaultPosition,
			wxSize(this->FromDIP(32), this->FromDIP(32)),
			wxBORDER_NONE
		);
		btn->SetBackgroundColour(Theme::Get().color.card);
		btn->SetToolTip(tip);
		return btn;
	};

	this->addButton = makeBtn(ID_HOME_ADD, 0, "Add deck");
	this->gridButton = makeBtn(ID_HOME_GRID, 1, "Grid view");
	this->listButton = makeBtn(ID_HOME_LIST, 2, "List view");

	// Floating pill cluster
	auto* pill = new wxPanel(this->toolbar);
	pill->SetBackgroundColour(Theme::Get().color.card);
	auto* pillSizer = new wxBoxSizer(wxHORIZONTAL);
	pillSizer->Add(this->addButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, this->FromDIP(4));
	pillSizer->Add(this->gridButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, this->FromDIP(4));
	pillSizer->Add(this->listButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, this->FromDIP(4));
	pill->SetSizer(pillSizer);

	this->search = new wxSearchCtrl(
		this->toolbar,
		ID_HOME_SEARCH,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(this->FromDIP(240), this->FromDIP(28)),
		wxTE_PROCESS_ENTER
	);
	this->search->SetDescriptiveText("Search decks");
	this->search->ShowSearchButton(true);
	this->search->ShowCancelButton(true);

	tb->AddSpacer(this->FromDIP(12));
	tb->Add(this->titleLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, this->FromDIP(16));
	tb->Add(pill, 0, wxALIGN_CENTER_VERTICAL);
	tb->AddStretchSpacer(1);
	tb->Add(this->search, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, this->FromDIP(16));
	this->toolbar->SetSizer(tb);

	this->Bind(wxEVT_BUTTON, &DeckHomePanel::OnAdd, this, ID_HOME_ADD);
	this->Bind(wxEVT_BUTTON, &DeckHomePanel::OnGrid, this, ID_HOME_GRID);
	this->Bind(wxEVT_BUTTON, &DeckHomePanel::OnList, this, ID_HOME_LIST);
	this->Bind(wxEVT_TEXT, &DeckHomePanel::OnSearch, this, ID_HOME_SEARCH);
	this->Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &DeckHomePanel::OnSearch, this, ID_HOME_SEARCH);
	this->Bind(wxEVT_TEXT_ENTER, &DeckHomePanel::OnSearch, this, ID_HOME_SEARCH);
}

void DeckHomePanel::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	this->toolbar->SetBackgroundColour(theme.color.window);
	this->titleLabel->SetForegroundColour(theme.color.label);
	this->scroll->SetBackgroundColour(theme.color.window);
	this->cardsHost->SetBackgroundColour(theme.color.window);
	this->Refresh();
}

void DeckHomePanel::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	dc.SetBackground(wxBrush(Theme::Get().color.window));
	dc.Clear();
}

void DeckHomePanel::LoadDecks() {
	this->decks.clear();
	auto useCase = wxGetApp().GetInjector().create<GetDecksUseCase>();
	GetDecksResponse response = useCase.Execute();
	for (const DeckResponse& deck : response.decks) {
		DeckItem item;
		item.deckId = deck.deckId;
		item.name = wxString(deck.name);
		item.description = wxString(deck.description);
		item.createdAt = wxString(deck.createdAt);
		item.cardCount = deck.cardCount;
		this->decks.push_back(item);
	}
	this->RebuildCards();
}

void DeckHomePanel::SetFilterQuery(const wxString& q) {
	this->query = q;
	if (this->search != nullptr && this->search->GetValue() != q) {
		this->search->ChangeValue(q);
	}
	this->RebuildCards();
}

void DeckHomePanel::RebuildCards() {
	this->cardsHost->DestroyChildren();
	this->wrap = new wxWrapSizer(wxHORIZONTAL);
	this->listSizer = new wxBoxSizer(wxVERTICAL);

	const wxString q = this->query.Lower();
	int colorIndex = 0;
	for (const DeckItem& deck : this->decks) {
		if (!q.empty() &&
			!deck.name.Lower().Contains(q) &&
			!deck.description.Lower().Contains(q)) {
			continue;
		}
		auto* card = new PastelDeckCard(
			this->cardsHost,
			deck.deckId,
			deck.name,
			deck.description,
			deck.createdAt,
			deck.cardCount,
			colorIndex++,
			[this](int id) { this->OpenDeck(id); }
		);
		if (this->gridMode) {
			this->wrap->Add(card, 0, wxALL, this->FromDIP(8));
		} else {
			card->SetMinSize(wxSize(-1, PastelDeckCard::kPreferredHeight));
			card->SetMaxSize(wxSize(-1, PastelDeckCard::kPreferredHeight));
			this->listSizer->Add(card, 0, wxEXPAND | wxALL, this->FromDIP(8));
		}
	}

	if (this->gridMode) {
		this->cardsHost->SetSizer(this->wrap);
	} else {
		this->cardsHost->SetSizer(this->listSizer);
	}
	this->cardsHost->Layout();
	this->scroll->FitInside();
	this->scroll->Refresh();
	this->Layout();
}

void DeckHomePanel::OnSearch(wxCommandEvent&) {
	this->query = this->search->GetValue();
	this->RebuildCards();
}

void DeckHomePanel::OnAdd(wxCommandEvent& event) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->OnAddDeck(event);
		this->LoadDecks();
	}
}

void DeckHomePanel::OnGrid(wxCommandEvent&) {
	this->gridMode = true;
	this->RebuildCards();
}

void DeckHomePanel::OnList(wxCommandEvent&) {
	this->gridMode = false;
	this->RebuildCards();
}

void DeckHomePanel::OpenDeck(int deckId) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->OnDeckSelected(deckId);
		if (auto* decks = dynamic_cast<DeckPanelList*>(frame->sidebarPanel)) {
			decks->SelectDeck(deckId, false);
		}
		frame->ShowCardList();
	}
}
