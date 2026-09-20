#ifndef CARD_LIST_PANEL_H
#define CARD_LIST_PANEL_H

#include "data_structures.h"
#include "dialog.h"
#include <vector>
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

class CardListPanel : public wxPanel {
	public:
		CardListPanel(wxWindow* parent, int deckID);
		
		int deckID;
		std::vector<Card> cards;
		
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
		
		void OnPaint(wxPaintEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
};

#endif
