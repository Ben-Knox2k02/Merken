#ifndef FLASH_CARD_LIST_H
#define FLASH_CARD_LIST_H

#include <wx/wx.h>
#include <vector>
#include "flash_card.h"

class FlashCardList : public wxPanel {
	public:
		FlashCardList(wxWindow* parent, int deckId);

		wxBoxSizer* rootSizer;
		wxBoxSizer* buttonSizer;
		wxStaticText* header;
		wxScrolledWindow* scroller;
		wxBoxSizer* listSizer;
		wxButton* addButton;
		wxButton* editButton;
		wxButton* deleteButton;
		wxButton* studyButton;

		int deckId;

		void SetDeck(int deckId);
		void LoadCards();
		int GetSelectedCardId() const;

		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnStudy(wxCommandEvent& event);

	private:
		std::vector<FlashCard*> cards;
		std::vector<int> cardIds;
		int selectedCardId;

		void AddCard(int cardId, const wxString& front, const wxString& back, const wxString& tags);
		void BindClicks(wxWindow* window, int cardId);
		void SelectCard(int cardId);
		void RefreshSelection();
		FlashCard* FindCard(int cardId) const;
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);

		static constexpr int kCornerRadius = 5;
		static constexpr int kMargin = 10;
};

#endif
