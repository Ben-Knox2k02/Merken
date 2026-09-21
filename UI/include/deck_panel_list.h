#ifndef DECK_PANEL_LIST_H
#define DECK_PANEL_LIST_H

#include <wx/wx.h>
#include <vector>
#include "deck_card.h"
#include "icon_button.h"

class DeckPanelList : public wxPanel {
	public:
		explicit DeckPanelList(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxScrolledWindow* scroller;
		wxBoxSizer* listSizer;
		IconButton* addDeckButton;
		IconButton* editDeckButton;
		IconButton* deleteDeckButton;

		void LoadDecks();
		int GetSelectedDeckId() const;
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

		void AddDeckCard(int deckId, const wxString& name);
		void BindClicks(wxWindow* window, int deckId);
		void RefreshSelection();
		DeckCard* FindCard(int deckId) const;
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
};

#endif
