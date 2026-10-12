#ifndef DECK_PANEL_LIST_H
#define DECK_PANEL_LIST_H

#include <wx/wx.h>
#include <vector>
#include "deck_card_icon.h"

class DeckPanelList : public wxPanel {
	public:
		explicit DeckPanelList(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxScrolledWindow* scroller;
		wxBoxSizer* listSizer;
		wxButton* addDeckButton;
		wxButton* editDeckButton;
		wxButton* deleteDeckButton;

		void LoadDecks();
		int GetSelectedDeckId() const;
		void SelectDeck(int deckId, bool notify = false);
		void NotifyDeckSelected(int deckId);

		void OnAddDeck(wxCommandEvent& event);
		void OnEditDeck(wxCommandEvent& event);
		void OnDeleteDeck(wxCommandEvent& event);

	private:
		std::vector<DeckCardIcon*> cards;
		std::vector<int> cardIds;
		int selectedDeckId;

		void AddDeckCard(int deckId, const wxString& name, const wxString& description);
		void BindClicks(wxWindow* window, int deckId);
		void RefreshSelection();
		DeckCardIcon* FindCard(int deckId) const;
};

#endif
