#ifndef DECK_PANEL_LIST_H
#define DECK_PANEL_LIST_H

#include "mainframe.h"
#include "deck_card.h"
#include "icon_button.h"
#include "profile_header.h"
#include "dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "guide_highlight.h"
#include <wx/dcgraph.h>
#include <wx/statline.h>
#include <vector>

class DeckPanelList : public wxPanel {
	public:
		explicit DeckPanelList(wxWindow* parent);
		
		App& app;

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

		static constexpr int kMinWidth = 320 * 2 / 3;

		void AddDeckCard(int deckId, const wxString& name, int cardCount);
		void BindClicks(wxWindow* window, int deckId);
		void RefreshSelection();
		DeckCard* FindCard(int deckId) const;
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
};

#endif
