#ifndef FLASH_CARD_LIST_H
#define FLASH_CARD_LIST_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/bmpbndl.h>

class FlashCardList : public wxPanel {
	public:
		FlashCardList(wxWindow* parent, int deckId);

		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxDataViewCtrl* cardList;
		wxDataViewListStore* cardViewModel;
		wxButton* addButton;
		wxButton* editButton;
		wxButton* deleteButton;
		wxButton* studyButton;

		int deckId;

		void SetDeck(int deckId);
		void LoadCards();
		int GetSelectedRow() const;
		int GetSelectedCardId() const;

		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnStudy(wxCommandEvent& event);

	private:
		wxBitmapBundle cardIcon;

		void AppendCard(int cardId, const wxString& front, const wxString& back, const wxString& tags);
		wxBitmapBundle LoadCardIcon();
};

#endif
