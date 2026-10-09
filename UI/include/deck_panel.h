#ifndef DECK_PANEL_H
#define DECK_PANEL_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

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

		int GetSelectedDeckId() const;
		void LoadDecks();
		void SelectDeck(int deckId);
		void NotifyDeckSelected(int deckId);

		void OnPaint(wxPaintEvent& event);
		void OnAddDeck(wxCommandEvent& event);
		void OnEditDeck(wxCommandEvent& event);
		void OnDeleteDeck(wxCommandEvent& event);
		void OnDeckActivated(wxDataViewEvent& event);
		void OnDeckSelectionChanged(wxDataViewEvent& event);
};

#endif
