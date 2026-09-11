#ifndef CARD_LIST_PANEL_H
#define CARD_LIST_PANEL_H

#include "card_dialog.h"
#include <wx/wx.h>
#include <wx/dataview.h>

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
		
		void LoadCards();
		int GetSelectedRow() const;
		
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
};

#endif
