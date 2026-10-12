#ifndef FLASH_CARD_LIST_H
#define FLASH_CARD_LIST_H

#include <wx/wx.h>
#include <vector>
#include "flash_card.h"

class FlashCardList : public wxPanel {
	public:
		FlashCardList(wxWindow* parent, int deckId);

		wxBoxSizer* rootSizer;
		wxBoxSizer* headerButtonSizer;
		wxStaticText* header;
		wxStaticText* description;
		wxScrolledWindow* scroller;
		wxFlexGridSizer* listSizer;
		wxButton* addButton;
		wxButton* studyButton;

		int deckId;

		void SetDeck(int deckId);
		void LoadCards();

		void OnAdd(wxCommandEvent& event);
		void OnStudy(wxCommandEvent& event);

	private:
		std::vector<FlashCard*> cards;
		std::vector<int> cardIds;
		wxString deckName;
		wxString deckDescription;
		int lastHeaderWrap;

		void AddCard(int cardId, const wxString& front, const wxString& back, const wxString& tags);
		void EditCard(int cardId);
		void DeleteCard(int cardId);
		FlashCard* FindCard(int cardId) const;
		void UpdateDeckHeader();
		void WrapHeader();
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
};

#endif
