#include "flash_card_list.h"
#include "dialog.h"
#include "centered_message.h"
#include "mainframe.h"
#include "theme.h"
#include "guide_highlight.h"
#include "flash_card_tag.h"
#include "app.h"
#include "../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"
#include <wx/renderer.h>
#include <wx/statline.h>
#include <wx/tokenzr.h>

wxDECLARE_APP(App);

namespace {

wxString WrapToWidth(wxWindow* win, const wxString& text, int width) {
	wxString result;
	wxStringTokenizer paragraphs(text, "\n", wxTOKEN_RET_EMPTY);
	bool firstParagraph = true;
	while (paragraphs.HasMoreTokens()) {
		const wxString paragraph = paragraphs.GetNextToken();
		wxString line;
		wxStringTokenizer words(paragraph, " \t");
		while (words.HasMoreTokens()) {
			const wxString word = words.GetNextToken();
			if (word.empty()) {
				continue;
			}
			const wxString trial = line.empty() ? word : line + " " + word;
			if (!line.empty() && win->GetTextExtent(trial).GetWidth() > width) {
				if (!result.empty() || !firstParagraph) {
					result += "\n";
				}
				result += line;
				line = word;
				firstParagraph = false;
			} else {
				line = trial;
			}
		}
		if (!line.empty()) {
			if (!result.empty() || !firstParagraph) {
				result += "\n";
			}
			result += line;
			firstParagraph = false;
		}
	}
	return result.empty() ? text : result;
}

wxString WrapToWidth(wxDC& dc, const wxString& text, int width) {
	wxString result;
	wxStringTokenizer paragraphs(text, "\n", wxTOKEN_RET_EMPTY);
	bool firstParagraph = true;
	while (paragraphs.HasMoreTokens()) {
		const wxString paragraph = paragraphs.GetNextToken();
		wxString line;
		wxStringTokenizer words(paragraph, " \t");
		while (words.HasMoreTokens()) {
			const wxString word = words.GetNextToken();
			if (word.empty()) {
				continue;
			}
			const wxString trial = line.empty() ? word : line + " " + word;
			if (!line.empty() && dc.GetTextExtent(trial).GetWidth() > width) {
				if (!result.empty() || !firstParagraph) {
					result += "\n";
				}
				result += line;
				line = word;
				firstParagraph = false;
			} else {
				line = trial;
			}
		}
		if (!line.empty()) {
			if (!result.empty() || !firstParagraph) {
				result += "\n";
			}
			result += line;
			firstParagraph = false;
		}
	}
	return result.empty() ? text : result;
}

int LabelHeight(wxWindow* win, const wxString& wrapped) {
	const int lineH = win->GetTextExtent("Ag").GetHeight();
	if (wrapped.empty()) {
		return lineH;
	}
	int lines = 1;
	for (size_t i = 0; i < wrapped.length(); ++i) {
		if (wrapped[i] == '\n') {
			++lines;
		}
	}
	return lines * lineH;
}

int LineCount(const wxString& wrapped) {
	if (wrapped.empty()) {
		return 1;
	}
	int lines = 1;
	for (size_t i = 0; i < wrapped.length(); ++i) {
		if (wrapped[i] == '\n') {
			++lines;
		}
	}
	return lines;
}

wxString TruncateSentences(const wxString& text) {
	wxString trimmed = text;
	trimmed.Trim(true).Trim(false);
	int sentences = 0;
	for (size_t i = 0; i < trimmed.length(); ++i) {
		const wxUniChar ch = trimmed[i];
		if (ch != '.' && ch != '!' && ch != '?') {
			continue;
		}

		size_t end = i + 1;
		while (end < trimmed.length() && (trimmed[end] == '"' || trimmed[end] == '\'' || trimmed[end] == ')')) {
			++end;
		}

		const bool atEnd = end >= trimmed.length();
		const bool atBreak = !atEnd && (
			trimmed[end] == ' ' ||
			trimmed[end] == '\n' ||
			trimmed[end] == '\r' ||
			trimmed[end] == '\t'
		);
		if (!atEnd && !atBreak) {
			continue;
		}

		++sentences;
		if (sentences < 3) {
			continue;
		}
		if (atEnd) {
			return trimmed;
		}

		wxString result = trimmed.Mid(0, end);
		result.Trim(true);
		return result + "...";
	}
	return trimmed;
}

void DrawLines(wxDC& dc, const wxString& wrapped, int x, int y, int lineH) {
	wxString line;
	int row = 0;
	for (size_t i = 0; i < wrapped.length(); ++i) {
		if (wrapped[i] == '\n') {
			dc.DrawText(line, x, y + row * lineH);
			line.clear();
			++row;
			continue;
		}
		line += wrapped[i];
	}
	dc.DrawText(line, x, y + row * lineH);
}

void DrawAction(wxWindow* window, wxDC& dc, const wxRect& rect, const wxString& label, bool enabled, bool hover, bool pressed) {
	int flags = 0;
	if (!enabled) {
		flags |= wxCONTROL_DISABLED;
	} else if (pressed) {
		flags |= wxCONTROL_PRESSED;
	} else if (hover) {
		flags |= wxCONTROL_CURRENT;
	}
	wxRendererNative::Get().DrawPushButton(window, dc, rect, flags);

	const Theme& theme = Theme::Get();
	wxColour text = theme.dark
		? theme.color.label
		: wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
	if (!enabled) {
		text = theme.dark
			? theme.color.tertiaryLabel
			: wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
	}
	dc.SetTextForeground(text);
	const wxSize extent = dc.GetTextExtent(label);
	dc.DrawText(
		label,
		rect.x + (rect.width - extent.GetWidth()) / 2,
		rect.y + (rect.height - extent.GetHeight()) / 2
	);
}

int Wider(int left, int right) {
	return left > right ? left : right;
}

// Tighter than the shared theme tokens, which the deck list still uses.
// The gutter between cards sits between the old 12 and the tighter 8.
const int kCardListGapDip = 10;

int CardGap(const wxWindow* window, int dip) {
	int tight = (dip * 2) / 3;
	if (tight < 2) {
		tight = 2;
	}
	return window->FromDIP(tight);
}

}

// The themed dark scrollbar track is near black. On this page that reads as a
// dim band down the right side, so repaint the track in the window colour.
void PaintScrollTrack(wxWindow* window) {
#ifdef __WXMSW__
	if (window == nullptr || window->GetHWND() == nullptr || window->IsBeingDeleted()) {
		return;
	}
	HWND hwnd = reinterpret_cast<HWND>(window->GetHWND());
	SCROLLINFO info = {};
	info.cbSize = sizeof(info);
	info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	if (::GetScrollInfo(hwnd, SB_VERT, &info) == FALSE) {
		return;
	}
	const int range = info.nMax - info.nMin + 1;
	if (range < 1 || info.nPage == 0 || range <= static_cast<int>(info.nPage)) {
		return;
	}

	RECT windowRect;
	::GetWindowRect(hwnd, &windowRect);
	RECT client;
	::GetClientRect(hwnd, &client);
	POINT origin = { 0, 0 };
	::ClientToScreen(hwnd, &origin);
	const int clientW = client.right - client.left;
	const int clientH = client.bottom - client.top;
	const int barX = (origin.x - windowRect.left) + clientW;
	const int barY = origin.y - windowRect.top;
	const int barW = (windowRect.right - windowRect.left) - barX;
	if (barW < 2 || clientH < 2) {
		return;
	}

	const Theme& theme = Theme::Get();
	int thumbH = static_cast<int>(static_cast<long long>(clientH) * static_cast<int>(info.nPage) / range);
	const int minThumb = window->FromDIP(28);
	if (thumbH < minThumb) {
		thumbH = minThumb;
	}
	if (thumbH > clientH) {
		thumbH = clientH;
	}
	const int maxPos = range - static_cast<int>(info.nPage);
	int thumbY = barY;
	if (maxPos > 0) {
		int pos = info.nPos - info.nMin;
		if (pos < 0) {
			pos = 0;
		}
		if (pos > maxPos) {
			pos = maxPos;
		}
		const int travel = clientH - thumbH;
		thumbY = barY + static_cast<int>(static_cast<long long>(travel) * pos / maxPos);
	}
	int thumbW = window->FromDIP(4);
	if (thumbW > barW - 2) {
		thumbW = barW - 2;
	}
	if (thumbW < 2) {
		thumbW = barW;
	}
	const int thumbX = barX + (barW - thumbW) / 2;

	static bool painting = false;
	if (painting) {
		return;
	}
	painting = true;
	wxWindowDC dc(window);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(theme.color.window));
	dc.DrawRectangle(barX, barY, barW, clientH);
	dc.SetBrush(wxBrush(theme.color.tertiaryLabel));
	dc.DrawRoundedRectangle(thumbX, thumbY, thumbW, thumbH, thumbW / 2);
	painting = false;
#else
	(void)window;
#endif
}

class CardCanvas : public wxScrolledWindow {
	public:
		explicit CardCanvas(FlashCardList* owner)
			: wxScrolledWindow(),
			  owner(owner) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->Create(owner, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxBORDER_NONE);
			this->SetBackgroundColour(Theme::Get().color.window);
			this->SetScrollRate(0, owner->FromDIP(16));
			this->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
			this->Bind(wxEVT_PAINT, &CardCanvas::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
			this->Bind(wxEVT_SIZE, &FlashCardList::OnScrollerSize, owner);
			this->Bind(wxEVT_LEFT_DOWN, &CardCanvas::OnLeftDown, this);
			this->Bind(wxEVT_LEFT_UP, &CardCanvas::OnLeftUp, this);
			this->Bind(wxEVT_MOTION, &CardCanvas::OnMotion, this);
			this->Bind(wxEVT_LEAVE_WINDOW, &CardCanvas::OnLeave, this);
			this->Bind(wxEVT_MOUSE_CAPTURE_LOST, &CardCanvas::OnCaptureLost, this);
		}

	private:
		FlashCardList* owner;

		void OnPaint(wxPaintEvent&) { this->owner->PaintCards(); }
#ifdef __WXMSW__
		WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override {
			const WXLRESULT result = wxScrolledWindow::MSWWindowProc(message, wParam, lParam);
			if (message == WM_NCPAINT || message == WM_VSCROLL || message == WM_MOUSEWHEEL || message == WM_SIZE) {
				PaintScrollTrack(this);
			}
			return result;
		}
#endif
		void OnLeftDown(wxMouseEvent& event) { this->owner->OnCanvasDown(event); }
		void OnLeftUp(wxMouseEvent& event) { this->owner->OnCanvasUp(event); }
		void OnMotion(wxMouseEvent& event) { this->owner->OnCanvasMotion(event); }
		void OnLeave(wxMouseEvent& event) { this->owner->OnCanvasLeave(event); }
		void OnCaptureLost(wxMouseCaptureLostEvent& event) { this->owner->OnCanvasCaptureLost(event); }
};

FlashCardList::FlashCardList(wxWindow* parent, int deckId)
	: wxPanel(),
	  deckId(deckId),
	  headerGap(nullptr),
	  lastHeaderWrap(0),
	  lastDescWrap(0),
	  measuredWidth(0),
	  contentHeight(0),
	  loadGeneration(0),
	  hoverIndex(-1),
	  pressedIndex(-1),
	  hoverHit(CardHit::None),
	  pressedHit(CardHit::None),
	  cardsEnabled(true),
	  inLayout(false) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->SetBackgroundColour(Theme::Get().color.window);

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText();
	this->header->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->header->Create(this, wxID_ANY, "");
	wxFont titleFont = this->header->GetFont();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	this->header->SetFont(titleFont);
	this->header->SetForegroundColour(Theme::Get().color.label);
	this->header->SetMinSize(wxSize(0, -1));

	this->description = new wxStaticText();
	this->description->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	this->description->Create(this, wxID_ANY, "");
	this->description->SetForegroundColour(Theme::Get().color.secondaryLabel);
	this->description->SetMinSize(wxSize(0, -1));
	this->description->Hide();

	const int pad = this->FromDIP(Theme::Get().size.panelPad);

	this->headerButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	this->addButton = new wxButton(this, wxID_ANY, "Add Flash Card");
	this->studyButton = new wxButton(this, wxID_ANY, "Study Deck");
	this->aiStudyButton = new wxButton(this, wxID_ANY, "AI Study Deck");
	ApplyButtonPadding(this->addButton);
	ApplyButtonPadding(this->studyButton);
	ApplyButtonPadding(this->aiStudyButton);
	int sharedWidth = this->addButton->GetMinSize().GetWidth();
	if (this->studyButton->GetMinSize().GetWidth() > sharedWidth) {
		sharedWidth = this->studyButton->GetMinSize().GetWidth();
	}
	if (this->aiStudyButton->GetMinSize().GetWidth() > sharedWidth) {
		sharedWidth = this->aiStudyButton->GetMinSize().GetWidth();
	}
	const wxSize sharedSize(sharedWidth, wxButton::GetDefaultSize(this).GetHeight());
	this->addButton->SetMinSize(sharedSize);
	this->studyButton->SetMinSize(sharedSize);
	this->aiStudyButton->SetMinSize(sharedSize);
	this->headerButtonSizer->Add(this->addButton, 0, wxRIGHT | wxALIGN_BOTTOM, this->FromDIP(Theme::Get().space.xs));
	this->headerButtonSizer->Add(this->studyButton, 0, wxRIGHT | wxALIGN_BOTTOM, this->FromDIP(Theme::Get().space.xs));
	this->headerButtonSizer->Add(this->aiStudyButton, 0);

	wxBoxSizer* headerRow = new wxBoxSizer(wxHORIZONTAL);
	headerRow->Add(this->header, 1, wxEXPAND | wxRIGHT, this->FromDIP(Theme::Get().space.md));
	headerRow->Add(this->headerButtonSizer, 0, wxALIGN_TOP);

	wxBoxSizer* headerBlock = new wxBoxSizer(wxVERTICAL);
	headerBlock->Add(headerRow, 0, wxEXPAND);
	this->rootSizer->Add(headerBlock, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().space.sm));
	this->rootSizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND);
	this->rootSizer->Add(
		this->description,
		0,
		wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM,
		this->FromDIP(Theme::Get().space.xxl)
	);
	const int nameToDesc = this->FromDIP(Theme::Get().space.xxl);
	this->headerGap = this->rootSizer->AddSpacer(nameToDesc > pad ? nameToDesc - pad : 0);
	this->headerGap->Show(false);

	this->scroller = new CardCanvas(this);
	this->listSizer = nullptr;
	this->leftCol = nullptr;
	this->rightCol = nullptr;

	this->emptyPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->emptyPanel->SetBackgroundColour(Theme::Get().color.window);
	this->emptyMessage = new wxStaticText(this->emptyPanel, wxID_ANY, "No flash cards yet");
	this->emptyMessage->SetForegroundColour(Theme::Get().color.secondaryLabel);
	wxBoxSizer* emptySizer = new wxBoxSizer(wxVERTICAL);
	emptySizer->AddStretchSpacer(1);
	emptySizer->Add(this->emptyMessage, 0, wxALIGN_CENTER_HORIZONTAL);
	emptySizer->AddStretchSpacer(1);
	this->emptyPanel->SetSizer(emptySizer);

	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
	this->rootSizer->Add(this->emptyPanel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
	this->SetSizer(this->rootSizer);

	this->addButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAdd, this);
	this->studyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnStudy, this);
	this->aiStudyButton->Bind(wxEVT_BUTTON, &FlashCardList::OnAiStudy, this);
	this->Bind(wxEVT_PAINT, &FlashCardList::OnPaint, this);
	this->Bind(wxEVT_SIZE, &FlashCardList::OnSize, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->LoadCards();
	std::cout<<"FlashCardList constructor executed"<<std::endl;
}

void FlashCardList::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	const wxSize size = this->GetClientSize();
	if (size.GetWidth() < 2 || size.GetHeight() < 2) {
		return;
	}

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(Theme::Get().CanvasBehind(this)));
	dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
}

void FlashCardList::OnSize(wxSizeEvent& event) {
	const int previousTitle = this->lastHeaderWrap;
	const int previousDesc = this->lastDescWrap;
	this->WrapHeader();
	if (this->lastHeaderWrap != previousTitle || this->lastDescWrap != previousDesc) {
		this->Layout();
	}
	event.Skip();
}

void FlashCardList::OnScrollerSize(wxSizeEvent& event) {
	event.Skip();
	if (this->items.empty() || this->inLayout) {
		return;
	}
	const int generation = this->loadGeneration;
	this->CallAfter([this, generation]() {
		if (generation != this->loadGeneration || this->items.empty() || this->inLayout) {
			return;
		}
		const int width = this->ColumnWidth();
		if (width > 0 && width != this->measuredWidth) {
			this->Remeasure();
		}
	});
}

void FlashCardList::SetDeck(int deckId) {
	this->deckId = deckId;
	this->LoadCards();
}

void FlashCardList::LoadCards() {
	this->loadGeneration += 1;
	this->ClearPointer();
	this->items.clear();
	this->cardIds.clear();
	this->measuredWidth = 0;
	this->contentHeight = 0;
	this->metrics.ready = false;

	std::cout<<"[TRACE] Updating deck header..."<<std::endl;
	this->UpdateDeckHeader();

	std::cout<<"[TRACE] Checking deckId"<<this->deckId<<std::endl;
	if (this->deckId != 0) {
		std::cout<<"[TRACE] Creating GetCardsUseCase..."<<std::endl;
		auto useCase = wxGetApp().GetInjector().create<GetCardsUseCase>();
		std::cout<<"[TRACE] Executing GetCardsUseCase..."<<std::endl;
		GetCardsResponse response = useCase.Execute(GetCardsRequest{this->deckId});
		this->items.reserve(response.cards.size());
		for (const CardResponse& card : response.cards) {
			CardItem item;
			item.cardId = card.cardId;
			item.front = wxString(card.front);
			item.back = wxString(card.back);
			item.tags = wxString(card.tags);
			this->items.push_back(item);
			this->cardIds.push_back(card.cardId);
		}
	}

	this->ShowEmptyState(this->items.empty());
	this->UpdateStudyButtons();
	this->RefreshGuide();
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->SetSelectedCardCount(static_cast<int>(this->items.size()));
	}

	if (this->items.empty()) {
		this->scroller->SetVirtualSize(0, 0);
		return;
	}

	const int generation = this->loadGeneration;
	this->CallAfter([this, generation]() {
		if (generation != this->loadGeneration) {
			return;
		}
		this->lastHeaderWrap = 0;
		this->lastDescWrap = 0;
		this->WrapHeader();
		this->Layout();
		this->measuredWidth = 0;
		this->scroller->Scroll(0, 0);
		this->Remeasure();
	});
	
	std::cout<<"[TRACE] LoadCards() executed successfully..."<<std::endl;
}

int FlashCardList::ColumnWidth() const {
	if (this->scroller == nullptr) {
		return 0;
	}
	int inner = this->scroller->GetClientSize().GetWidth();
	if (this->items.size() > 6 && !this->scroller->HasScrollbar(wxVERTICAL)) {
		const int bar = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, this->scroller);
		if (bar > 0 && inner > bar) {
			inner -= bar;
		}
	}
	const int gap = this->FromDIP(kCardListGapDip);
	const int halfGap = gap / 2;
	if (inner <= halfGap * 2) {
		return 0;
	}
	return (inner - halfGap * 2) / 2;
}

int FlashCardList::ScrollY() const {
	if (this->scroller == nullptr) {
		return 0;
	}
	int x = 0;
	int y = 0;
	this->scroller->GetViewStartPixels(&x, &y);
	return y;
}

void FlashCardList::BuildMetrics(wxDC& dc, int columnWidth) {
	const Theme& theme = Theme::Get();
	CardMetrics& m = this->metrics;
	m.pad = CardGap(this, theme.size.cardPad);
	m.gridVGap = CardGap(this, theme.space.md);
	m.gridHGap = CardGap(this, theme.space.lg);
	m.fieldVGap = CardGap(this, theme.space.lg);
	m.fieldHGap = CardGap(this, theme.space.xl);
	m.captionTop = CardGap(this, theme.space.xs);
	m.wellPadX = CardGap(this, theme.size.wellPadX);
	m.wellPadY = CardGap(this, theme.size.wellPadY);
	m.badge = this->FromDIP(theme.size.badge);
	m.cardRadius = this->FromDIP(theme.radius.xs);
	m.badgeRadius = m.cardRadius;
	m.wellRadius = this->FromDIP(theme.radius.well);
	m.actionGap = CardGap(this, theme.space.xs);
	m.tagGap = CardGap(this, theme.space.sm);
	m.listGap = this->FromDIP(kCardListGapDip);
	m.columnGap = (m.listGap / 2) * 2;
	m.columnW = columnWidth;
	m.lineH = dc.GetTextExtent("Ag").GetHeight();
	if (m.lineH < 1) {
		m.lineH = 1;
	}
	m.captionW = dc.GetTextExtent("Front:").GetWidth();
	m.captionW = Wider(m.captionW, dc.GetTextExtent("Back:").GetWidth());
	m.captionW = Wider(m.captionW, dc.GetTextExtent("Tags:").GetWidth());
	const int labelW = Wider(dc.GetTextExtent("Edit").GetWidth(), dc.GetTextExtent("Delete").GetWidth());
	m.buttonW = labelW + this->FromDIP(15) * 2;
	m.buttonH = wxButton::GetDefaultSize(this->scroller).GetHeight();
	if (m.buttonH < m.lineH) {
		m.buttonH = m.lineH;
	}
	m.ruleH = 2;
	m.fieldsW = columnWidth - 2 * m.pad - m.badge - m.gridHGap;
	if (m.fieldsW < 1) {
		m.fieldsW = 1;
	}
	m.col1W = m.fieldsW - m.captionW - m.fieldHGap;
	if (m.col1W < 1) {
		m.col1W = 1;
	}
	m.textW = m.col1W - 2 * m.wellPadX;
	if (m.textW < 1) {
		m.textW = 1;
	}
	m.ready = true;
}

int FlashCardList::MeasureItem(wxDC& dc, CardItem& item) {
	item.frontWrapped = WrapToWidth(dc, TruncateSentences(item.front), this->metrics.textW);
	item.backWrapped = WrapToWidth(dc, TruncateSentences(item.back), this->metrics.textW);
	item.tagLabels = FlashCardTag::Split(item.tags);
	const FieldLayout layout = this->LayoutAt(item, 0, 0, dc);
	item.editX = layout.edit.x;
	item.editY = layout.edit.y;
	item.deleteX = layout.remove.x;
	item.deleteY = layout.remove.y;
	return layout.card.GetHeight();
}

FlashCardList::FieldLayout FlashCardList::LayoutAt(const CardItem& item, int x, int y, wxDC& dc) const {
	const CardMetrics& m = this->metrics;
	FieldLayout layout;
	const int contentX = x + m.pad;
	const int contentY = y + m.pad;
	const int fieldsX = contentX + m.badge + m.gridHGap;
	const int wellX = fieldsX + m.captionW + m.fieldHGap;

	const int captionH = m.captionTop + m.lineH;
	const int frontTextH = LineCount(item.frontWrapped) * m.lineH;
	const int backTextH = LineCount(item.backWrapped) * m.lineH;
	const int frontRow = Wider(captionH, frontTextH + 2 * m.wellPadY);
	const int backRow = Wider(captionH, backTextH + 2 * m.wellPadY);

	int fieldsH = frontRow + m.fieldVGap + backRow;
	int tagRow = 0;
	layout.hasTags = !item.tagLabels.empty();
	if (layout.hasTags) {
		int chipX = 0;
		int rowH = 0;
		int blockH = 0;
		for (size_t i = 0; i < item.tagLabels.size(); ++i) {
			const wxSize chip = FlashCardTag::Measure(dc, item.tagLabels[i], this->scroller);
			const int itemW = chip.GetWidth() + m.tagGap;
			const int itemH = chip.GetHeight() + m.tagGap;
			if (chipX > 0 && chipX + itemW > m.col1W) {
				blockH += rowH;
				chipX = 0;
				rowH = 0;
			}
			wxRect chipRect(wellX + chipX, blockH, chip.GetWidth(), chip.GetHeight());
			layout.chips.push_back(chipRect);
			chipX += itemW;
			rowH = Wider(rowH, itemH);
		}
		blockH += rowH;
		tagRow = Wider(captionH, blockH);
		fieldsH += m.fieldVGap + tagRow;
	}

	const int row0 = Wider(m.badge, fieldsH);
	const int ruleY = contentY + row0 + m.gridVGap;
	const int actionY = ruleY + m.ruleH + m.gridVGap;
	const int height = (actionY - y) + m.buttonH + m.pad;
	const int gridW = m.badge + m.gridHGap + m.fieldsW;

	layout.card = wxRect(x, y, m.columnW, height);
	layout.badge = wxRect(contentX, contentY, m.badge, m.badge);
	layout.frontCaption = wxPoint(fieldsX, contentY + m.captionTop);
	layout.frontWell = wxRect(wellX, contentY, m.col1W, frontRow);
	const int backY = contentY + frontRow + m.fieldVGap;
	layout.backCaption = wxPoint(fieldsX, backY + m.captionTop);
	layout.backWell = wxRect(wellX, backY, m.col1W, backRow);
	if (layout.hasTags) {
		const int tagsY = backY + backRow + m.fieldVGap;
		layout.tagsCaption = wxPoint(fieldsX, tagsY + m.captionTop);
		for (wxRect& chip : layout.chips) {
			chip.y = tagsY + (chip.y);
		}
	}
	layout.ruleX = contentX;
	layout.ruleY = ruleY;
	layout.ruleW = gridW;
	const int right = contentX + gridW;
	layout.remove = wxRect(right - m.buttonW, actionY, m.buttonW, m.buttonH);
	layout.edit = wxRect(layout.remove.x - m.actionGap - m.buttonW, actionY, m.buttonW, m.buttonH);
	return layout;
}

void FlashCardList::Remeasure() {
	if (this->inLayout || this->scroller == nullptr || this->items.empty()) {
		return;
	}
	const int width = this->ColumnWidth();
	const int minWrap = this->FromDIP(Theme::Get().size.minWrap);
	if (width < minWrap) {
		return;
	}

	this->inLayout = true;
	wxClientDC dc(this->scroller);
	dc.SetFont(this->scroller->GetFont());
	this->BuildMetrics(dc, width);
	int leftTop = 0;
	int rightTop = 0;
	for (size_t i = 0; i < this->items.size(); ++i) {
		CardItem& item = this->items[i];
		item.left = i % 2 == 0;
		item.height = this->MeasureItem(dc, item);
		item.top = item.left ? leftTop : rightTop;
		if (item.left) {
			leftTop += item.height + this->metrics.listGap;
		} else {
			rightTop += item.height + this->metrics.listGap;
		}
	}
	this->contentHeight = Wider(leftTop, rightTop);
	this->measuredWidth = width;
	// Width 0 keeps the horizontal bar hidden after the vertical bar appears.
	this->scroller->SetVirtualSize(0, this->contentHeight);
	this->inLayout = false;
	this->scroller->Refresh();
}

void FlashCardList::PaintCards() {
	wxPaintDC dc(this->scroller);
	const wxRect box = this->scroller->GetUpdateRegion().GetBox();
	if (box.IsEmpty()) {
		return;
	}

	// ScrollWindow already moved the pixels. Draw only the new strip, with no buffer.
	dc.DestroyClippingRegion();
	dc.SetClippingRegion(box);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(Theme::Get().color.window));
	dc.DrawRectangle(box);
	dc.DestroyClippingRegion();

	if (!this->metrics.ready || this->items.empty()) {
		return;
	}

	dc.SetFont(this->scroller->GetFont());
	const int scrollY = this->ScrollY();
	for (size_t i = 0; i < this->items.size(); ++i) {
		const CardItem& item = this->items[i];
		const int x = item.left ? 0 : this->metrics.columnW + this->metrics.columnGap;
		const int y = item.top - scrollY;
		const wxRect cardRect(x, y, this->metrics.columnW, item.height);
		const wxRect part = cardRect.Intersect(box);
		if (part.IsEmpty()) {
			continue;
		}
		dc.SetClippingRegion(part);
		const FieldLayout layout = this->LayoutAt(item, x, y, dc);
		this->DrawCard(dc, layout, i);
		dc.DestroyClippingRegion();
	}
}

void FlashCardList::DrawCard(wxDC& dc, const FieldLayout& layout, size_t index) {
	const Theme& theme = Theme::Get();
	const CardMetrics& m = this->metrics;
	const CardItem& item = this->items[index];
	const wxFont bodyFont = dc.GetFont();
	const bool enabled = this->cardsEnabled;
	const int cardIndex = static_cast<int>(index);

	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(theme.color.card));
	dc.DrawRoundedRectangle(
		layout.card.x,
		layout.card.y,
		layout.card.width - 1,
		layout.card.height - 1,
		m.cardRadius
	);

	dc.SetBrush(wxBrush(theme.color.fill));
	dc.DrawRoundedRectangle(
		layout.badge.x,
		layout.badge.y,
		layout.badge.width - 1,
		layout.badge.height - 1,
		m.badgeRadius
	);

	const wxString number = wxString::Format("%d", cardIndex + 1);
	wxFont badgeFont = bodyFont;
	const int limit = m.badge - 2;
	for (int point = badgeFont.GetPointSize(); point >= 6; --point) {
		badgeFont.SetPointSize(point);
		dc.SetFont(badgeFont);
		const wxSize extent = dc.GetTextExtent(number);
		if (extent.GetWidth() <= limit && extent.GetHeight() <= limit) {
			break;
		}
	}
	dc.SetTextForeground(theme.color.label);
	const wxSize numberExtent = dc.GetTextExtent(number);
	dc.DrawText(
		number,
		layout.badge.x + (layout.badge.width - numberExtent.GetWidth()) / 2,
		layout.badge.y + (layout.badge.height - numberExtent.GetHeight()) / 2
	);
	dc.SetFont(bodyFont);

	auto drawWell = [&](const wxRect& well) {
		if (well.GetWidth() < 2 || well.GetHeight() < 2) {
			return;
		}
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(theme.color.separator, theme.stroke.hairline));
		dc.DrawRoundedRectangle(well.x, well.y, well.width - 1, well.height - 1, m.wellRadius);
	};

	dc.SetTextForeground(theme.color.label);
	dc.DrawText("Front:", layout.frontCaption.x, layout.frontCaption.y);
	drawWell(layout.frontWell);
	DrawLines(dc, item.frontWrapped, layout.frontWell.x + m.wellPadX, layout.frontWell.y + m.wellPadY, m.lineH);

	dc.SetTextForeground(theme.color.label);
	dc.DrawText("Back:", layout.backCaption.x, layout.backCaption.y);
	drawWell(layout.backWell);
	DrawLines(dc, item.backWrapped, layout.backWell.x + m.wellPadX, layout.backWell.y + m.wellPadY, m.lineH);

	if (layout.hasTags) {
		dc.SetTextForeground(theme.color.label);
		dc.DrawText("Tags:", layout.tagsCaption.x, layout.tagsCaption.y);
		dc.SetFont(bodyFont);
		const size_t count = layout.chips.size() < item.tagLabels.size()
			? layout.chips.size()
			: item.tagLabels.size();
		for (size_t i = 0; i < count; ++i) {
			FlashCardTag::Draw(dc, layout.chips[i], item.tagLabels[i], FlashCardTag::Style::Default);
		}
		dc.SetFont(bodyFont);
	}

	dc.SetPen(wxPen(theme.color.separator, theme.stroke.hairline));
	dc.DrawLine(layout.ruleX, layout.ruleY, layout.ruleX + layout.ruleW, layout.ruleY);

	const bool editHover = enabled && this->hoverIndex == cardIndex && this->hoverHit == CardHit::Edit;
	const bool editPressed = enabled && this->pressedIndex == cardIndex && this->pressedHit == CardHit::Edit;
	const bool deleteHover = enabled && this->hoverIndex == cardIndex && this->hoverHit == CardHit::Delete;
	const bool deletePressed = enabled && this->pressedIndex == cardIndex && this->pressedHit == CardHit::Delete;
	DrawAction(this->scroller, dc, layout.edit, "Edit", enabled, editHover, editPressed);
	DrawAction(this->scroller, dc, layout.remove, "Delete", enabled, deleteHover, deletePressed);
}

FlashCardList::CardHit FlashCardList::HitTest(const wxPoint& point, size_t& index) const {
	if (this->scroller == nullptr || !this->metrics.ready || this->items.empty()) {
		return CardHit::None;
	}
	const int scrollY = this->ScrollY();
	const int viewH = this->scroller->GetClientSize().GetHeight();
	for (size_t i = 0; i < this->items.size(); ++i) {
		const CardItem& item = this->items[i];
		const int x = item.left ? 0 : this->metrics.columnW + this->metrics.columnGap;
		const int y = item.top - scrollY;
		if (item.height < 1 || y + item.height < 0 || y > viewH) {
			continue;
		}
		const wxRect edit(x + item.editX, y + item.editY, this->metrics.buttonW, this->metrics.buttonH);
		const wxRect remove(x + item.deleteX, y + item.deleteY, this->metrics.buttonW, this->metrics.buttonH);
		if (edit.Contains(point)) {
			index = i;
			return CardHit::Edit;
		}
		if (remove.Contains(point)) {
			index = i;
			return CardHit::Delete;
		}
	}
	return CardHit::None;
}

void FlashCardList::UpdateHover(const wxPoint& point) {
	if (!this->cardsEnabled) {
		this->SetHover(-1, CardHit::None);
		return;
	}
	size_t index = 0;
	const CardHit hit = this->HitTest(point, index);
	this->SetHover(hit == CardHit::None ? -1 : static_cast<int>(index), hit);
}

void FlashCardList::SetHover(int index, CardHit hit) {
	if (index == this->hoverIndex && hit == this->hoverHit) {
		return;
	}
	const int previous = this->hoverIndex;
	this->hoverIndex = index;
	this->hoverHit = hit;
	this->RefreshCard(previous);
	this->RefreshCard(index);
	if (this->scroller != nullptr) {
		const bool hand = this->cardsEnabled && hit != CardHit::None;
		this->scroller->SetCursor(hand ? wxCursor(wxCURSOR_HAND) : wxCursor(wxCURSOR_ARROW));
	}
}

void FlashCardList::RefreshCard(int index) {
	if (this->scroller == nullptr || index < 0 || static_cast<size_t>(index) >= this->items.size()) {
		return;
	}
	const CardItem& item = this->items[index];
	if (item.height < 1 || !this->metrics.ready) {
		return;
	}
	const int x = item.left ? 0 : this->metrics.columnW + this->metrics.columnGap;
	const int y = item.top - this->ScrollY();
	this->scroller->RefreshRect(wxRect(x, y, this->metrics.columnW, item.height), false);
}

void FlashCardList::ClearPointer() {
	this->hoverIndex = -1;
	this->pressedIndex = -1;
	this->hoverHit = CardHit::None;
	this->pressedHit = CardHit::None;
	if (this->scroller != nullptr && this->scroller->HasCapture()) {
		this->scroller->ReleaseMouse();
	}
	if (this->scroller != nullptr) {
		this->scroller->SetCursor(wxCursor(wxCURSOR_ARROW));
	}
}

void FlashCardList::OnCanvasDown(wxMouseEvent& event) {
	if (this->scroller != nullptr) {
		this->scroller->SetFocus();
	}
	size_t index = 0;
	const CardHit hit = this->HitTest(event.GetPosition(), index);
	if (!this->cardsEnabled || hit == CardHit::None) {
		event.Skip();
		return;
	}
	this->pressedIndex = static_cast<int>(index);
	this->pressedHit = hit;
	if (this->scroller != nullptr && !this->scroller->HasCapture()) {
		this->scroller->CaptureMouse();
	}
	this->RefreshCard(this->pressedIndex);
}

void FlashCardList::OnCanvasUp(wxMouseEvent& event) {
	if (this->scroller != nullptr && this->scroller->HasCapture()) {
		this->scroller->ReleaseMouse();
	}
	const int index = this->pressedIndex;
	const CardHit hit = this->pressedHit;
	this->pressedIndex = -1;
	this->pressedHit = CardHit::None;
	if (index >= 0) {
		this->RefreshCard(index);
	}
	if (!this->cardsEnabled || hit == CardHit::None || index < 0) {
		this->UpdateHover(event.GetPosition());
		return;
	}
	size_t under = 0;
	const CardHit now = this->HitTest(event.GetPosition(), under);
	if (now != hit || static_cast<int>(under) != index) {
		this->UpdateHover(event.GetPosition());
		return;
	}
	const int cardId = this->items[static_cast<size_t>(index)].cardId;
	if (hit == CardHit::Edit) {
		this->EditCard(cardId);
		return;
	}
	this->DeleteCard(cardId);
}

void FlashCardList::OnCanvasMotion(wxMouseEvent& event) {
	this->UpdateHover(event.GetPosition());
	event.Skip();
}

void FlashCardList::OnCanvasLeave(wxMouseEvent&) {
	if (this->pressedHit != CardHit::None) {
		return;
	}
	this->SetHover(-1, CardHit::None);
}

void FlashCardList::OnCanvasCaptureLost(wxMouseCaptureLostEvent&) {
	const int index = this->pressedIndex;
	this->pressedIndex = -1;
	this->pressedHit = CardHit::None;
	this->RefreshCard(index);
}

void FlashCardList::RefreshGuide() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = useCase.Execute();
	const bool guiding = profile.ok && !profile.guideFinished;
	const bool addStep = guiding && this->deckId != 0 && this->items.empty();
	const bool studyStep = guiding && this->deckId != 0 && !this->items.empty();
	this->cardsEnabled = !guiding;
	if (!this->cardsEnabled) {
		this->ClearPointer();
	}
	if (this->scroller != nullptr) {
		this->scroller->Refresh();
	}
	if (!guiding) {
		this->addButton->Enable(true);
		this->UpdateStudyButtons();
		GuideHighlight::SetBorder(this->addButton, false);
		GuideHighlight::SetBorder(this->studyButton, false);
		return;
	}
	this->addButton->Enable(addStep);
	this->studyButton->Enable(studyStep);
	this->aiStudyButton->Enable(false);
	if (addStep) {
		GuideHighlight::SetBorder(this->studyButton, false);
		GuideHighlight::Announce(
			this->addButton,
			GuidePrompt::AddCard,
			"Let's add a flash-card to the deck. Write the front and the back, and add any tags that help you remember."
		);
		return;
	}
	if (studyStep) {
		GuideHighlight::SetBorder(this->addButton, false);
		GuideHighlight::Announce(
			this->studyButton,
			GuidePrompt::StudyDeck,
			"Let's study it together. The front comes up first, and then you can reveal the back."
		);
		return;
	}
	GuideHighlight::SetBorder(this->addButton, false);
	GuideHighlight::SetBorder(this->studyButton, false);
}

void FlashCardList::UpdateDeckHeader() {
	this->deckName = wxEmptyString;
	this->deckDescription = wxEmptyString;

	if (this->deckId != 0) {
		auto useCase = wxGetApp().GetInjector().create<GetDecksUseCase>();
		GetDecksResponse response = useCase.Execute();
		for (const DeckResponse& deck : response.decks) {
			if (deck.deckId == this->deckId) {
				this->deckName = wxString(deck.name);
				this->deckDescription = wxString(deck.description);
				this->deckDescription.Trim(true).Trim(false);
				break;
			}
		}
	}

	this->header->SetLabel(this->deckName);
	this->header->Show(!this->deckName.IsEmpty());
	const bool hasDeck = this->deckId != 0;
	this->headerButtonSizer->Show(this->addButton, hasDeck);
	this->headerButtonSizer->Show(this->studyButton, hasDeck);
	this->headerButtonSizer->Show(this->aiStudyButton, hasDeck);
	this->description->SetLabel(this->deckDescription);
	const bool hasDescription = !this->deckDescription.IsEmpty();
	this->description->Show(hasDescription);
	if (this->headerGap != nullptr) {
		const int nameToDesc = this->FromDIP(Theme::Get().space.xxl);
		const int pad = this->FromDIP(Theme::Get().size.panelPad);
		const int missing = !hasDescription && nameToDesc > pad ? nameToDesc - pad : 0;
		this->headerGap->SetMinSize(wxSize(0, missing));
		this->headerGap->Show(missing > 0);
	}
	this->lastHeaderWrap = 0;
	this->lastDescWrap = 0;
	this->WrapHeader();
	this->Layout();
}

void FlashCardList::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	this->header->SetForegroundColour(theme.color.label);
	this->description->SetForegroundColour(theme.color.secondaryLabel);
	this->scroller->SetBackgroundColour(theme.color.window);
	if (this->emptyPanel != nullptr) {
		this->emptyPanel->SetBackgroundColour(theme.color.window);
	}
	if (this->emptyMessage != nullptr) {
		this->emptyMessage->SetForegroundColour(theme.color.secondaryLabel);
	}

	this->RefreshGuide();
	this->Refresh();
	if (this->scroller != nullptr) {
		this->scroller->Refresh();
		PaintScrollTrack(this->scroller);
	}
}

void FlashCardList::ShowEmptyState(bool empty) {
	if (this->rootSizer != nullptr) {
		this->rootSizer->Show(this->scroller, !empty);
		this->rootSizer->Show(this->emptyPanel, empty);
	}
	this->Layout();
}

void FlashCardList::UpdateStudyButtons() {
	const bool canStudy = this->deckId != 0 && !this->items.empty();
	this->studyButton->Enable(canStudy);
	this->aiStudyButton->Enable(canStudy);
}

void FlashCardList::WrapHeader() {
	const int pad = this->FromDIP(Theme::Get().size.panelPad);
	const int minWrap = this->FromDIP(Theme::Get().size.minWrap);
	const int buttons = this->headerButtonSizer != nullptr ? this->headerButtonSizer->GetMinSize().GetWidth() : 0;

	int titleWrap = this->header->GetClientSize().GetWidth();
	if (titleWrap < minWrap) {
		titleWrap = this->GetClientSize().GetWidth() - 2 * pad - buttons - this->FromDIP(Theme::Get().space.md);
	}
	if (titleWrap >= minWrap && this->lastHeaderWrap != titleWrap) {
		this->lastHeaderWrap = titleWrap;
		const wxString title = WrapToWidth(this->header, this->deckName, titleWrap);
		this->header->SetLabel(title);
		this->header->SetMinSize(wxSize(0, LabelHeight(this->header, title)));
	}

	if (!this->description->IsShown()) {
		return;
	}
	int descWrap = this->description->GetClientSize().GetWidth();
	if (descWrap < minWrap) {
		descWrap = this->GetClientSize().GetWidth() - 2 * pad;
	}
	if (descWrap < minWrap || this->lastDescWrap == descWrap) {
		return;
	}
	this->lastDescWrap = descWrap;
	const wxString desc = WrapToWidth(this->description, this->deckDescription, descWrap);
	this->description->SetLabel(desc);
	this->description->SetMinSize(wxSize(0, LabelHeight(this->description, desc)));
}

const FlashCardList::CardItem* FlashCardList::FindItem(int cardId) const {
	for (const CardItem& item : this->items) {
		if (item.cardId == cardId) {
			return &item;
		}
	}
	return nullptr;
}

void FlashCardList::OnAdd(wxCommandEvent&) {
	if (this->deckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	CardDialog dialog(this, "Add Card");
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string front = dialog.frontCtrl->GetValue().ToStdString();
	const std::string back = dialog.backCtrl->GetValue().ToStdString();
	if (front.empty() || back.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Add Card", wxOK | wxICON_WARNING);
		return;
	}

	CreateCardRequest request;
	request.deckId = this->deckId;
	request.front = front;
	request.back = back;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<CreateCardUseCase>();
	CreateCardResponse response = useCase.Execute(request);
	if (response.cardId == 0) {
		ShowCenteredMessage(this, "Could not save card.", "Add Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ReloadDecks();
	}
}

void FlashCardList::EditCard(int cardId) {
	const CardItem* card = this->FindItem(cardId);
	if (card == nullptr) { return; }

	CardDialog dialog(this, "Edit Card");
	dialog.frontCtrl->SetValue(card->front);
	dialog.backCtrl->SetValue(card->back);
	dialog.tagCtrl->SetValue(card->tags);
	if (dialog.ShowModal() != wxID_OK) { return; }

	const std::string newFront = dialog.frontCtrl->GetValue().ToStdString();
	const std::string newBack = dialog.backCtrl->GetValue().ToStdString();
	if (newFront.empty() || newBack.empty()) {
		ShowCenteredMessage(this, "Front and back are required.", "Edit Card", wxOK | wxICON_WARNING);
		return;
	}

	UpdateCardRequest request;
	request.deckId = this->deckId;
	request.cardId = cardId;
	request.front = newFront;
	request.back = newBack;
	request.tags = dialog.tagCtrl->GetValue().ToStdString();

	auto useCase = wxGetApp().GetInjector().create<UpdateCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not update card.", "Edit Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
}

void FlashCardList::DeleteCard(int cardId) {
	if (this->deckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Delete Card", wxOK | wxICON_WARNING);
		return;
	}

	const int confirmed = ShowCenteredMessage(
		this,
		"Delete this card?",
		"Delete Card",
		wxYES_NO | wxICON_QUESTION,
		nullptr,
		nullptr,
		true
	);
	if (confirmed != wxID_YES) { return; }

	DeleteCardRequest request;
	request.deckId = this->deckId;
	request.cardId = cardId;
	auto useCase = wxGetApp().GetInjector().create<DeleteCardUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not delete card.", "Delete Card", wxOK | wxICON_ERROR);
		return;
	}

	this->LoadCards();
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ReloadDecks();
	}
}

void FlashCardList::OnStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowStudyDeck();
	}
}

void FlashCardList::OnAiStudy(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowAiStudy();
	}
}
