#ifndef DECK_PANEL_H
#define DECK_PANEL_H

#include "dialog.h"
#include <vector>
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

struct Deck {
	int ID;
	wxString name;
	wxString description;
};

class DeckPanel : public wxPanel {
	public:
		DeckPanel(wxWindow* parent);
		
		std::vector<Deck> decks;
		Deck* activeDeck;
		
		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxDataViewCtrl* deckList;
		wxDataViewListStore* deckViewModel;
		wxButton* addDeckButton;
		wxButton* editDeckButton;
		wxButton* deleteDeckButton;
		
		void LoadDecks();
		void Print();
		int GetSelectedRow() const;
		
		void OnPaint(wxPaintEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnDeckActivated(wxDataViewEvent& event);
};

#endif
