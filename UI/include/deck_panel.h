#ifndef DECK_PANEL_H
#define DECK_PANEL_H

#include <wx/wx.h>
#include <wx/dataview.h>

class DeckPanel : public wxPanel {
	public:
		DeckPanel(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxDataViewCtrl* deckList;
		wxDataViewListStore* deckViewModel;
		wxButton* addDeckButton;
		wxButton* editDeckButton;
		wxButton* deleteDeckButton;

		void LoadDecks();
		int GetSelectedDeckId() const;
		void SelectDeck(int deckId);
		void NotifyDeckSelected(int deckId);

		void OnAddDeck(wxCommandEvent& event);
		void OnEditDeck(wxCommandEvent& event);
		void OnDeleteDeck(wxCommandEvent& event);
		void OnDeckActivated(wxDataViewEvent& event);
		void OnDeckSelectionChanged(wxDataViewEvent& event);
};

#endif
