#ifndef DECK_HOME_PANEL_H
#define DECK_HOME_PANEL_H

#include <wx/wx.h>
#include <wx/srchctrl.h>
#include <wx/wrapsizer.h>
#include <wx/scrolwin.h>
#include <vector>
#include <string>

class PastelDeckCard;

class DeckHomePanel : public wxPanel {
	public:
		explicit DeckHomePanel(wxWindow* parent);

		void LoadDecks();
		void ApplyTheme();
		void SetFilterQuery(const wxString& query);

	private:
		struct DeckItem {
			int deckId = 0;
			wxString name;
			wxString description;
			wxString createdAt;
			int cardCount = 0;
		};

		wxPanel* toolbar = nullptr;
		wxStaticText* titleLabel = nullptr;
		wxSearchCtrl* search = nullptr;
		wxBitmapButton* addButton = nullptr;
		wxBitmapButton* gridButton = nullptr;
		wxBitmapButton* listButton = nullptr;
		wxScrolledWindow* scroll = nullptr;
		wxPanel* cardsHost = nullptr;
		wxWrapSizer* wrap = nullptr;
		wxBoxSizer* listSizer = nullptr;
		bool gridMode = true;
		wxString query;
		std::vector<DeckItem> decks;

		void BuildToolbar();
		void RebuildCards();
		void OnSearch(wxCommandEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnGrid(wxCommandEvent& event);
		void OnList(wxCommandEvent& event);
		void OpenDeck(int deckId);
		void OnPaint(wxPaintEvent& event);
		static wxBitmap MakeToolBitmap(int kind, int size = 18);
};

#endif
