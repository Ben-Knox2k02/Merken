#ifndef FLASH_CARD_LIST_H
#define FLASH_CARD_LIST_H

#include <wx/wx.h>
#include <vector>

class CardCanvas;

class FlashCardList : public wxPanel {
	public:
		FlashCardList(wxWindow* parent, int deckId);

		wxBoxSizer* rootSizer;
		wxBoxSizer* headerButtonSizer;
		wxStaticText* header;
		wxStaticText* description;
		wxScrolledWindow* scroller;
		wxPanel* emptyPanel;
		wxStaticText* emptyMessage;
		wxBoxSizer* listSizer;
		wxBoxSizer* leftCol;
		wxBoxSizer* rightCol;
		wxButton* addButton;
		wxButton* studyButton;
		wxButton* aiStudyButton;
		wxButton* eventsButton;

		int deckId;

		void SetDeck(int deckId);
		void LoadCards();
		void ApplyTheme();
		void RefreshGuide();

		void OnAdd(wxCommandEvent& event);
		void OnStudy(wxCommandEvent& event);
		void OnAiStudy(wxCommandEvent& event);
		void OnToggleEvents(wxCommandEvent& event);

	private:
		friend class CardCanvas;

		enum class CardHit {
			None = 0,
			Edit = 1,
			Delete = 2
		};

		struct CardItem {
			int cardId = 0;
			wxString front;
			wxString back;
			wxString tags;
			wxString frontWrapped;
			wxString backWrapped;
			wxArrayString tagLabels;
			int top = 0;
			int height = 0;
			int editX = 0;
			int editY = 0;
			int deleteX = 0;
			int deleteY = 0;
			bool left = true;
		};

		struct CardMetrics {
			int pad = 0;
			int gridVGap = 0;
			int gridHGap = 0;
			int fieldVGap = 0;
			int fieldHGap = 0;
			int captionTop = 0;
			int wellPadX = 0;
			int wellPadY = 0;
			int badge = 0;
			int cardRadius = 0;
			int badgeRadius = 0;
			int wellRadius = 0;
			int actionGap = 0;
			int tagGap = 0;
			int listGap = 0;
			int columnGap = 0;
			int columnW = 0;
			int lineH = 0;
			int captionW = 0;
			int fieldsW = 0;
			int col1W = 0;
			int textW = 0;
			int buttonW = 0;
			int buttonH = 0;
			int ruleH = 2;
			bool ready = false;
		};

		struct FieldLayout {
			wxRect card;
			wxRect badge;
			wxRect frontWell;
			wxRect backWell;
			wxRect edit;
			wxRect remove;
			wxPoint frontCaption;
			wxPoint backCaption;
			wxPoint tagsCaption;
			int ruleX = 0;
			int ruleY = 0;
			int ruleW = 0;
			bool hasTags = false;
			std::vector<wxRect> chips;
		};

		std::vector<CardItem> items;
		std::vector<int> cardIds;
		CardMetrics metrics;
		wxString deckName;
		wxString deckDescription;
		wxSizerItem* headerGap;
		int lastHeaderWrap;
		int lastDescWrap;
		int measuredWidth;
		int contentHeight;
		int loadGeneration;
		int hoverIndex;
		int pressedIndex;
		CardHit hoverHit;
		CardHit pressedHit;
		bool cardsEnabled;
		bool inLayout;

		void EditCard(int cardId);
		void DeleteCard(int cardId);
		const CardItem* FindItem(int cardId) const;
		void UpdateDeckHeader();
		void UpdateStudyButtons();
		void UpdateEventsButton();
		void ShowEmptyState(bool empty);
		void WrapHeader();
		void Remeasure();
		int ColumnWidth() const;
		int ScrollY() const;
		void BuildMetrics(wxDC& dc, int columnWidth);
		int MeasureItem(wxDC& dc, CardItem& item);
		FieldLayout LayoutAt(const CardItem& item, int x, int y, wxDC& dc) const;
		void PaintCards();
		void DrawCard(wxDC& dc, const FieldLayout& layout, size_t index);
		CardHit HitTest(const wxPoint& point, size_t& index) const;
		void UpdateHover(const wxPoint& point);
		void SetHover(int index, CardHit hit);
		void RefreshCard(int index);
		void ClearPointer();
		void OnCanvasDown(wxMouseEvent& event);
		void OnCanvasUp(wxMouseEvent& event);
		void OnCanvasMotion(wxMouseEvent& event);
		void OnCanvasLeave(wxMouseEvent& event);
		void OnCanvasCaptureLost(wxMouseCaptureLostEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnScrollerSize(wxSizeEvent& event);
};

#endif
