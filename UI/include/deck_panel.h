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
		wxButton* addDeckButton;
		wxButton* editDeckButton;
		wxButton* deleteDeckButton;
		
		void LoadDecks();
		void OnAddDeck(wxCommandEvent& event);
		void OnEditDeck(wxCommandEvent& event);
		void OnDeleteDeck(wxCommandEvent& event);
		void OnDeckActivated(wxDataViewEvent& event);
};

#endif
