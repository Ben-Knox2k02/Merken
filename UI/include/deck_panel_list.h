#ifndef DECK_PANEL_LIST_H
#define DECK_PANEL_LIST_H

#include <wx/wx.h>
#include <vector>
#include "deck_card.h"
#include "icon_button.h"
#include "profile_header.h"

class DeckPanelList : public wxPanel {
	public:
		explicit DeckPanelList(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxFlexGridSizer* buttonSizer;
		ProfileHeader* profileHeader;
		wxScrolledWindow* scroller;
		wxBoxSizer* listSizer;
		IconButton* addDeckButton;
		IconButton* editDeckButton;
		IconButton* deleteDeckButton;

		void LoadDecks();
		void ApplyTheme();
		void RefreshGuide();
		int GetSelectedDeckId() const;
		bool HasDecks() const { return !this->cards.empty(); }
		void SelectDeck(int deckId, bool notify = false);
		void NotifyDeckSelected(int deckId);

		void OnAddDeck(wxCommandEvent& event);
		void OnEditDeck(wxCommandEvent& event);
		void OnDeleteDeck(wxCommandEvent& event);

	private:
		std::vector<DeckCard*> cards;
		std::vector<int> cardIds;
		int selectedDeckId;
		int selectedNavId;
		wxPanel* settingsGear = nullptr;
		wxBoxSizer* contentSizer = nullptr;

		static constexpr int kMinWidth = 220;

		void BuildChrome();
		void AddSectionHeader(const wxString& title);
		void AddNavRow(const wxString& label, int navId, bool accentBookmark = false);
		void AddDeckCard(int deckId, const wxString& name, int cardCount);
		void BindClicks(wxWindow* window, int deckId);
		void RefreshSelection();
		DeckCard* FindCard(int deckId) const;
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnNav(int navId);
		void OnSettingsClick(wxMouseEvent& event);
};

#endif
