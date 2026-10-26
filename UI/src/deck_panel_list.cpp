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
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/statline.h>
#include <cmath>

wxDECLARE_APP(App);

namespace {
enum {
	NAV_HOME = 1,
	NAV_FAVORITES,
	NAV_CARDS,
	NAV_STUDY,
	NAV_CALENDAR,
	NAV_AI,
	NAV_PROGRESS
};

class SectionHeader : public wxPanel {
	public:
		SectionHeader(wxWindow* parent, const wxString& title)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 28), wxBORDER_NONE),
			  title(title) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->Bind(wxEVT_PAINT, &SectionHeader::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

	private:
		wxString title;
		void OnPaint(wxPaintEvent&) {
			wxAutoBufferedPaintDC dc(this);
			const wxSize sz = this->GetClientSize();
			const Theme& theme = Theme::Get();
			dc.SetBrush(wxBrush(theme.color.grouped));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(0, 0, sz.x, sz.y);
			dc.SetTextForeground(theme.color.tertiaryLabel);
			dc.SetFont(wxFontInfo(10).Bold().Family(wxFONTFAMILY_DEFAULT));
			dc.DrawText(this->title, this->FromDIP(20), (sz.y - dc.GetCharHeight()) / 2);
		}
};

class NavRow : public wxPanel {
	public:
		NavRow(wxWindow* parent, const wxString& label, int navId, bool accentBookmark)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 36), wxBORDER_NONE),
			  label(label),
			  navId(navId),
			  accentBookmark(accentBookmark),
			  hover(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->SetCursor(wxCURSOR_HAND);
			this->Bind(wxEVT_PAINT, &NavRow::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
			this->Bind(wxEVT_LEFT_DOWN, &NavRow::OnClick, this);
			this->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent&) { this->hover = true; this->Refresh(); });
			this->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent&) { this->hover = false; this->Refresh(); });
		}

		int GetNavId() const { return this->navId; }

	private:
		wxString label;
		int navId;
		bool accentBookmark;
		bool hover;

		void OnClick(wxMouseEvent&) {
			wxCommandEvent evt(wxEVT_BUTTON, this->GetId());
			evt.SetInt(this->navId);
			evt.SetEventObject(this);
			this->ProcessWindowEvent(evt);
		}

		void OnPaint(wxPaintEvent&) {
			wxAutoBufferedPaintDC dc(this);
			const wxSize sz = this->GetClientSize();
			const Theme& theme = Theme::Get();
			dc.SetBrush(wxBrush(theme.color.grouped));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(0, 0, sz.x, sz.y);

			if (this->hover) {
				dc.SetBrush(wxBrush(theme.color.fill));
				dc.DrawRoundedRectangle(this->FromDIP(8), this->FromDIP(2), sz.x - this->FromDIP(16), sz.y - this->FromDIP(4), this->FromDIP(8));
			}

			const int ix = this->FromDIP(20);
			const int iy = (sz.y - this->FromDIP(16)) / 2;
			if (this->accentBookmark) {
				dc.SetBrush(wxBrush(Theme::Hex(0x8C64DC)));
				dc.SetPen(*wxTRANSPARENT_PEN);
				wxPoint pts[5] = {
					wxPoint(ix, iy),
					wxPoint(ix + this->FromDIP(14), iy),
					wxPoint(ix + this->FromDIP(14), iy + this->FromDIP(18)),
					wxPoint(ix + this->FromDIP(7), iy + this->FromDIP(13)),
					wxPoint(ix, iy + this->FromDIP(18))
				};
				dc.DrawPolygon(5, pts);
			} else {
				dc.SetBrush(wxBrush(theme.color.tertiaryLabel));
				dc.SetPen(*wxTRANSPARENT_PEN);
				dc.DrawCircle(ix + this->FromDIP(6), iy + this->FromDIP(8), this->FromDIP(3));
			}

			dc.SetTextForeground(theme.color.label);
			dc.SetFont(wxFontInfo(11).Family(wxFONTFAMILY_DEFAULT));
			dc.DrawText(this->label, ix + this->FromDIP(22), (sz.y - dc.GetCharHeight()) / 2);
		}
};

class GearButton : public wxPanel {
	public:
		GearButton(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(36, 36), wxBORDER_NONE) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->SetCursor(wxCURSOR_HAND);
			this->Bind(wxEVT_PAINT, &GearButton::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

	private:
		void OnPaint(wxPaintEvent&) {
			wxAutoBufferedPaintDC dc(this);
			const wxSize sz = this->GetClientSize();
			const Theme& theme = Theme::Get();
			dc.SetBrush(wxBrush(theme.color.grouped));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(0, 0, sz.x, sz.y);

			const int cx = sz.x / 2;
			const int cy = sz.y / 2;
			dc.SetPen(wxPen(theme.color.tertiaryLabel, 2));
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawCircle(cx, cy, 8);
			dc.DrawCircle(cx, cy, 3);
			for (int i = 0; i < 8; ++i) {
				const double a = i * 3.14159265359 / 4.0;
				dc.DrawLine(
					cx + static_cast<int>(10 * cos(a)),
					cy + static_cast<int>(10 * sin(a)),
					cx + static_cast<int>(13 * cos(a)),
					cy + static_cast<int>(13 * sin(a))
				);
			}
		}
};
} // namespace

DeckPanelList::DeckPanelList(wxWindow* parent)
	: wxPanel(),
	  selectedDeckId(0),
	  selectedNavId(NAV_HOME) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->SetBackgroundColour(Theme::Get().color.grouped);
	this->SetMinSize(wxSize(this->FromDIP(kMinWidth), -1));

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->BuildChrome();
	this->SetSizer(this->rootSizer);

	this->addDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnAddDeck, this);
	this->editDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnEditDeck, this);
	this->deleteDeckButton->Bind(wxEVT_BUTTON, &DeckPanelList::OnDeleteDeck, this);
	this->Bind(wxEVT_PAINT, &DeckPanelList::OnPaint, this);
	this->Bind(wxEVT_SIZE, &DeckPanelList::OnSize, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->LoadDecks();
}

void DeckPanelList::BuildChrome() {
	const int pad = this->FromDIP(Theme::Get().size.cardPad);
	const int profileSpace = this->FromDIP(Theme::Get().space.xl);

	this->rootSizer->AddSpacer(profileSpace);
	this->profileHeader = new ProfileHeader(this);
	this->rootSizer->Add(this->profileHeader, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().space.md));

	this->contentSizer = new wxBoxSizer(wxVERTICAL);
	this->AddSectionHeader("Library");
	this->AddNavRow("Favorites", NAV_FAVORITES, true);
	this->AddNavRow("Home", NAV_HOME, false);
	this->AddNavRow("Cards", NAV_CARDS, false);
	this->AddNavRow("Study", NAV_STUDY, false);
	this->AddNavRow("Calendar", NAV_CALENDAR, false);
	this->AddNavRow("AI Study", NAV_AI, false);
	this->AddNavRow("Progress", NAV_PROGRESS, false);
	this->contentSizer->AddSpacer(this->FromDIP(8));
	this->AddSectionHeader("My Decks");

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetWindowStyleFlag(this->scroller->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->scroller->SetBackgroundColour(Theme::Get().color.grouped);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->contentSizer->Add(this->scroller, 1, wxEXPAND);
	this->rootSizer->Add(this->contentSizer, 1, wxEXPAND);

	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().space.md));

	this->addDeckButton = new IconButton(this, "UI/assets/add_icon.png", "Add");
	this->editDeckButton = new IconButton(this, "UI/assets/edit_icon.png", "Edit");
	this->deleteDeckButton = new IconButton(this, "UI/assets/delete_icon.png", "Delete");
	const int buttonHeight = this->addDeckButton->GetMinHeight();
	const wxSize sharedButtonSize(0, buttonHeight);
	this->addDeckButton->SetMinSize(sharedButtonSize);
	this->editDeckButton->SetMinSize(sharedButtonSize);
	this->deleteDeckButton->SetMinSize(sharedButtonSize);

	this->buttonSizer = new wxFlexGridSizer(1, 3, 0, this->FromDIP(Theme::Get().space.xs));
	this->buttonSizer->AddGrowableCol(0, 1);
	this->buttonSizer->AddGrowableCol(1, 1);
	this->buttonSizer->AddGrowableCol(2, 1);
	this->buttonSizer->SetFlexibleDirection(wxHORIZONTAL);
	this->buttonSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
	this->buttonSizer->Add(this->addDeckButton, 1, wxEXPAND);
	this->buttonSizer->Add(this->editDeckButton, 1, wxEXPAND);
	this->buttonSizer->Add(this->deleteDeckButton, 1, wxEXPAND);
	this->rootSizer->Add(this->buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);

	this->settingsGear = new GearButton(this);
	this->settingsGear->Bind(wxEVT_LEFT_DOWN, &DeckPanelList::OnSettingsClick, this);
	this->rootSizer->Add(this->settingsGear, 0, wxALIGN_LEFT | wxALL, this->FromDIP(12));
}

void DeckPanelList::AddSectionHeader(const wxString& title) {
	this->contentSizer->Add(new SectionHeader(this, title), 0, wxEXPAND);
}

void DeckPanelList::AddNavRow(const wxString& label, int navId, bool accentBookmark) {
	auto* row = new NavRow(this, label, navId, accentBookmark);
	row->Bind(wxEVT_BUTTON, [this](wxCommandEvent& e) {
		this->OnNav(e.GetInt());
	});
	this->contentSizer->Add(row, 0, wxEXPAND);
}

void DeckPanelList::OnNav(int navId) {
	this->selectedNavId = navId;
	auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this));
	if (frame == nullptr) {
		return;
	}
	switch (navId) {
		case NAV_HOME:
		case NAV_FAVORITES:
			frame->ShowDeckHome();
			break;
		case NAV_CARDS:
			frame->ShowCardList();
			break;
		case NAV_STUDY:
			frame->ShowStudyDeck();
			break;
		case NAV_CALENDAR:
			frame->ShowCalendar();
			break;
		case NAV_AI:
			frame->ShowAiStudy();
			break;
		case NAV_PROGRESS:
			frame->ShowTodaysProgress();
			break;
		default:
			break;
	}
}

void DeckPanelList::OnSettingsClick(wxMouseEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		wxCommandEvent evt(wxEVT_MENU, ID_SETTINGS);
		frame->OnSettings(evt);
	}
}

void DeckPanelList::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.grouped);
	this->profileHeader->ApplyTheme();
	this->scroller->SetBackgroundColour(theme.color.grouped);
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
	wxAutoBufferedPaintDC dc(this);
	const wxSize sz = this->GetClientSize();
	const Theme& theme = Theme::Get();
	dc.SetBrush(wxBrush(theme.color.grouped));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0, 0, sz.x, sz.y);
	dc.SetPen(wxPen(theme.color.separator, 1));
	dc.DrawLine(sz.x - 1, 0, sz.x - 1, sz.y);
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowDeckHome();
	}
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowDeckHome();
	}
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowDeckHome();
	}
}
