#ifndef CARD_LIST_PANEL_H
#define CARD_LIST_PANEL_H

#include "dialog.h"
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

class CardListPanel : public wxPanel {
	public:
		CardListPanel(wxWindow* parent, int deckID);

		int deckID;
		
		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		
		wxStaticText* header;
		
		wxDataViewCtrl* cardList;
		wxDataViewListStore* cardViewModel;
		
		wxButton* addButton;
		wxButton* editButton;
		wxButton* deleteButton;

		void SetDeck(int deckId);
		void LoadCards();
		
		int GetSelectedRow() const;
		int GetSelectedCardId() const;

		void OnPaint(wxPaintEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
};

#endif
