#ifndef CARD_LIST_PANEL_H
#define CARD_LIST_PANEL_H

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
		wxButton* addButton;
		wxButton* editButton;
		wxButton* deleteButton;
		
		void LoadCards();
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
};

#endif
