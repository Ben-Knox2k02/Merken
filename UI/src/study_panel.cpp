#include "study_panel.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "theme.h"
#include "guide_highlight.h"
#include "../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"
#include "../../Application/UseCases/Profile/FinishGuide/finish_guide_usecase.h"
#include <wx/dcgraph.h>
#include <wx/tokenzr.h>
#include <algorithm>
#include <cmath>

wxDECLARE_APP(App);

namespace {
wxString FormatPercent(double rate) {
	return wxString::Format("%.0f%%", rate * 100.0);
}

wxString WrapToWidth(wxWindow* window, const wxString& text, int width) {
	if (text.empty() || width < 1) {
		return text;
	}

	wxString result;
	wxStringTokenizer paragraphs(text, "\n", wxTOKEN_RET_EMPTY);
	bool firstParagraph = true;
	while (paragraphs.HasMoreTokens()) {
		const wxString paragraph = paragraphs.GetNextToken();
		if (!firstParagraph) {
			result += "\n";
		}
		firstParagraph = false;

		wxString line;
		bool wrappedLine = false;
		wxStringTokenizer words(paragraph, " ", wxTOKEN_STRTOK);
		while (words.HasMoreTokens()) {
			const wxString word = words.GetNextToken();
			const wxString trial = line.empty() ? word : line + " " + word;
			if (!line.empty() && window->GetTextExtent(trial).GetWidth() > width) {
				if (wrappedLine) {
					result += "\n";
				}
				result += line;
				line = word;
				wrappedLine = true;
			} else {
				line = trial;
			}
		}
		if (!line.empty()) {
			if (wrappedLine) {
				result += "\n";
			}
			result += line;
		}
	}
	return result.empty() ? text : result;
}
}

enum class StudyCardSide {
	Neutral,
	Front,
	Back
};

class SessionDonut : public wxPanel {
	public:
		SessionDonut(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  reviewed(0),
			  correct(0),
			  fill(Theme::Get().color.card) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			const int size = this->FromDIP(200);
			this->SetMinSize(wxSize(size, size));
			this->Bind(wxEVT_PAINT, &SessionDonut::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetFill(const wxColour& colour) {
			this->fill = colour;
			this->Refresh();
		}

		void SetStats(int reviewedCount, int correctCount) {
			this->reviewed = reviewedCount;
			this->correct = correctCount;
			this->Refresh();
		}

	private:
		int reviewed;
		int correct;
		wxColour fill;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			wxGraphicsContext* g = gc.GetGraphicsContext();
			const wxSize size = this->GetClientSize();
			if (g == nullptr || size.GetWidth() < 4 || size.GetHeight() < 4) {
				return;
			}

			g->SetPen(*wxTRANSPARENT_PEN);
			g->SetBrush(wxBrush(this->fill));
			g->DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			const double thickness = this->FromDIP(16);
			const double radius = std::min(size.GetWidth(), size.GetHeight()) / 2.0 - thickness;
			if (radius < thickness) {
				return;
			}
			const double cx = size.GetWidth() / 2.0;
			const double cy = size.GetHeight() / 2.0;
			constexpr double kPi = 3.141592653589793;
			const double start = -kPi / 2.0;
			const Theme& theme = Theme::Get();

			auto strokeArc = [&](const wxColour& colour, double from, double to) {
				wxGraphicsPath path = g->CreatePath();
				path.AddArc(cx, cy, radius, from, to, true);
				g->SetPen(g->CreatePen(wxGraphicsPenInfo(colour).Width(thickness).Cap(wxCAP_BUTT)));
				g->StrokePath(path);
			};
			auto strokeRing = [&](const wxColour& colour) {
				strokeArc(colour, start, start + kPi);
				strokeArc(colour, start + kPi, start + 2.0 * kPi);
			};

			strokeRing(theme.color.separator);
			if (this->reviewed > 0 && this->correct > 0) {
				const double sweep = (2.0 * kPi * this->correct) / this->reviewed;
				if (this->correct >= this->reviewed) {
					strokeRing(theme.color.success);
				} else {
					strokeArc(theme.color.success, start, start + sweep);
				}
			}

			const int percent = this->reviewed > 0
				? static_cast<int>(std::lround((100.0 * this->correct) / this->reviewed))
				: 0;
			wxFont font = this->GetFont();
			font.SetPointSize(font.GetPointSize() + 3);
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			g->SetFont(font, theme.color.label);
			double textWidth = 0;
			double textHeight = 0;
			const wxString label = wxString::Format("%d%% Retention", percent);
			g->GetTextExtent(label, &textWidth, &textHeight);
			g->DrawText(label, cx - textWidth / 2.0, cy - textHeight / 2.0);
		}
};

class StudyCard : public wxPanel {
	public:
		explicit StudyCard(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  baseWidth(0),
			  side(StudyCardSide::Neutral),
			  lastWrap(-1),
			  wrapping(false),
			  visualScale(1) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);

			const Theme& theme = Theme::Get();
			this->caption = new wxStaticText(
				this,
				wxID_ANY,
				"Front",
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_LEFT
			);
			this->caption->SetBackgroundStyle(wxBG_STYLE_COLOUR);
			this->caption->SetBackgroundColour(theme.color.card);
			this->caption->SetForegroundColour(theme.color.label);
			wxFont captionFont = this->caption->GetFont();
			captionFont.SetPointSize(captionFont.GetPointSize() + 3);
			captionFont.SetWeight(wxFONTWEIGHT_NORMAL);
			this->caption->SetFont(captionFont);

			this->textHost = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			this->textHost->SetBackgroundColour(theme.color.card);
			this->text = new wxStaticText(
				this->textHost,
				wxID_ANY,
				wxEmptyString,
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_LEFT
			);
			this->text->SetBackgroundColour(theme.color.card);
			this->text->SetForegroundColour(theme.color.label);
			wxFont font = this->text->GetFont();
			font.SetPointSize(font.GetPointSize() + 3);
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			this->text->SetFont(font);
			this->centeredText = new wxStaticText(
				this->textHost,
				wxID_ANY,
				wxEmptyString,
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL
			);
			this->centeredText->SetBackgroundColour(theme.color.card);
			this->centeredText->SetForegroundColour(theme.color.label);
			this->centeredText->SetFont(font);
			this->textSizer = new wxBoxSizer(wxVERTICAL);
			this->textSizer->Add(this->text, 1, wxEXPAND);
			this->textSizer->Add(this->centeredText, 1, wxEXPAND);
			this->textSizer->Show(this->centeredText, false);
			this->textHost->SetSizer(this->textSizer);

			const int pad = this->ContentPad();
			this->sizer = new wxBoxSizer(wxVERTICAL);
			this->sizer->Add(this->caption, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, pad);
			this->sizer->AddStretchSpacer(1);
			this->sizer->Add(this->textHost, 0, wxALIGN_CENTER_HORIZONTAL);
			this->notice = new wxStaticText(
				this,
				wxID_ANY,
				wxEmptyString,
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL
			);
			this->notice->SetFont(font);
			this->notice->SetForegroundColour(theme.color.label);
			this->notice->SetBackgroundColour(theme.color.card);
			this->sizer->Add(this->notice, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
			this->sizer->AddStretchSpacer(1);

			this->sessionTitle = new wxStaticText(
				this,
				wxID_ANY,
				"Session Complete",
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL
			);
			wxFont sessionFont = this->sessionTitle->GetFont();
			sessionFont.SetPointSize(sessionFont.GetPointSize() + 3);
			sessionFont.SetWeight(wxFONTWEIGHT_NORMAL);
			this->sessionTitle->SetFont(sessionFont);
			this->sessionTitle->SetForegroundColour(theme.color.label);
			this->sessionTitle->SetBackgroundColour(theme.color.card);
			this->donut = new SessionDonut(this);
			this->sizer->Insert(0, this->sessionTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxLEFT | wxRIGHT, pad);
			this->sizer->Insert(5, this->donut, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, pad);
			this->sizer->Show(this->caption, false);
			this->sizer->Show(this->notice, false);
			this->sizer->Show(this->sessionTitle, false);
			this->sizer->Show(this->donut, false);
			this->SetSizer(this->sizer);

			this->Bind(wxEVT_PAINT, &StudyCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &StudyCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void ShowSummary(int reviewed, int correct) {
			this->side = StudyCardSide::Neutral;
			this->ApplyColours();
			this->sizer->Show(this->caption, false);
			this->sizer->Show(this->textHost, false);
			this->sizer->Show(this->notice, false);
			this->sizer->Show(this->sessionTitle, true);
			this->sizer->Show(this->donut, true);
			this->sessionTitle->SetBackgroundColour(this->FillColour());
			this->sessionTitle->SetForegroundColour(Theme::Get().color.label);
			this->donut->SetFill(this->FillColour());
			this->donut->SetStats(reviewed, correct);
			this->Layout();
			this->Refresh();
		}

		void ShowNotice(const wxString& value) {
			this->side = StudyCardSide::Neutral;
			this->ApplyColours();
			this->sizer->Show(this->caption, false);
			this->sizer->Show(this->textHost, false);
			this->sizer->Show(this->sessionTitle, false);
			this->sizer->Show(this->donut, false);
			this->notice->SetLabel(value);
			this->notice->SetBackgroundColour(this->FillColour());
			this->notice->SetForegroundColour(Theme::Get().color.label);
			this->sizer->Show(this->notice, true);
			this->Layout();
			this->Refresh();
		}

		void SetText(const wxString& value, StudyCardSide side = StudyCardSide::Neutral) {
			this->sizer->Show(this->sessionTitle, false);
			this->sizer->Show(this->donut, false);
			this->sizer->Show(this->notice, false);
			this->visualScale = 1;
			this->side = side;
			this->raw = value;
			this->lastWrap = -1;
			this->ApplyColours();
			this->Rewrap();
			this->ShowFace();
			this->Refresh();
		}

		void SetBaseWidth(int width) {
			const int inner = std::max(1, width - 2 * this->ContentPad());
			if (this->baseWidth == inner) {
				return;
			}
			this->baseWidth = inner;
			this->textHost->SetMinSize(wxSize(inner, -1));
			this->textHost->SetMaxSize(wxSize(inner, -1));
			this->lastWrap = -1;
			this->Rewrap();
		}

		void SetVisualScale(double scale) {
			if (std::abs(scale - 1.0) < 0.001) {
				if (this->visualScale != 1.0) {
					this->visualScale = 1.0;
					this->lastWrap = -1;
					this->ShowFace();
					this->Rewrap();
				}
				return;
			}
			this->visualScale = scale;
			this->HideFace();
			this->Refresh();
		}

	private:
		wxBoxSizer* sizer;
		wxStaticText* caption;
		wxPanel* textHost;
		wxStaticText* text;
		wxStaticText* centeredText;
		wxBoxSizer* textSizer;
		wxStaticText* notice;
		wxStaticText* sessionTitle;
		SessionDonut* donut;
		int baseWidth;
		wxString raw;
		StudyCardSide side;
		int lastWrap;
		bool wrapping;
		double visualScale;

		int ContentPad() const {
			return this->FromDIP(Theme::Get().size.cardPad + 12);
		}

		void HideFace() {
			this->sizer->Show(this->caption, false);
			this->sizer->Show(this->textHost, false);
		}

		void ShowFace() {
			this->sizer->Show(this->textHost, true);
			this->sizer->Show(this->caption, this->side != StudyCardSide::Neutral);
			this->Layout();
		}

		void DrawScaled(wxGCDC& gc, int pageWidth, int pageHeight) {
			const Theme& theme = Theme::Get();
			const wxColour fill = this->FillColour();
			theme.DrawRounded(
				gc,
				wxRect(0, 0, pageWidth - 1, pageHeight - 1),
				theme.Dip(this, theme.radius.md),
				fill,
				fill
			);
			wxGraphicsContext* g = gc.GetGraphicsContext();
			if (g == nullptr) {
				return;
			}
			const int pad = this->ContentPad();
			const wxColour ink = theme.color.label;
			double y = pad;
			if (this->side != StudyCardSide::Neutral) {
				const wxString label = this->side == StudyCardSide::Back ? "Back" : "Front";
				g->SetFont(this->caption->GetFont(), ink);
				double width = 0;
				double height = 0;
				g->GetTextExtent(label, &width, &height);
				g->DrawText(label, pad, y);
				y += height + pad;
			}
			const int wrapWidth = std::max(pageWidth - 2 * pad, this->FromDIP(theme.size.minWrap));
			const wxString body = WrapToWidth(this->text, this->raw, wrapWidth);
			g->SetFont(this->text->GetFont(), ink);
			double lineWidth = 0;
			double lineHeight = 0;
			g->GetTextExtent("Ag", &lineWidth, &lineHeight);
			int lines = 1;
			for (size_t i = 0; i < body.length(); ++i) {
				if (body[i] == '\n') {
					++lines;
				}
			}
			const double blockHeight = lines * lineHeight;
			double lineY = y + std::max(0.0, (pageHeight - y - blockHeight) / 2.0);
			wxStringTokenizer rows(body, "\n", wxTOKEN_RET_EMPTY_ALL);
			while (rows.HasMoreTokens()) {
				const wxString row = rows.GetNextToken();
				g->DrawText(row, pad, lineY);
				lineY += lineHeight;
			}
		}

		wxColour FillColour() const {
			const Theme& theme = Theme::Get();
			if (this->side == StudyCardSide::Back) {
				return theme.color.studyBack;
			}
			if (this->side == StudyCardSide::Front) {
				return theme.color.studyFront;
			}
			return theme.color.card;
		}

		void ApplyColours() {
			const Theme& theme = Theme::Get();
			const wxColour fill = this->FillColour();
			const wxColour ink = theme.color.label;
			this->textHost->SetBackgroundColour(fill);
			this->text->SetBackgroundColour(fill);
			this->text->SetForegroundColour(ink);
			this->centeredText->SetBackgroundColour(fill);
			this->centeredText->SetForegroundColour(ink);
			this->caption->SetBackgroundColour(fill);
			this->caption->SetForegroundColour(ink);
			if (this->side == StudyCardSide::Back) {
				this->caption->SetLabel("Back");
				this->sizer->Show(this->caption, true);
			} else if (this->side == StudyCardSide::Front) {
				this->caption->SetLabel("Front");
				this->sizer->Show(this->caption, true);
			} else {
				this->sizer->Show(this->caption, false);
			}
			this->caption->Refresh();
			this->text->Refresh();
		}

		void Rewrap() {
			const int pad = this->ContentPad();
			int wrapWidth = this->baseWidth > 0 ? this->baseWidth : this->GetClientSize().GetWidth() - 2 * pad;
			const int minWrap = this->FromDIP(Theme::Get().size.minWrap);
			if (wrapWidth < minWrap) {
				if (this->lastWrap != -1) {
					this->text->SetLabel(this->raw);
					this->centeredText->SetLabel(this->raw);
					this->lastWrap = -1;
				}
				this->ApplyBodyAlignment();
				return;
			}
			if (this->lastWrap != wrapWidth) {
				this->lastWrap = wrapWidth;
				this->ApplyColours();
				const wxString wrapped = WrapToWidth(this->text, this->raw, wrapWidth);
				this->text->SetLabel(wrapped);
				this->centeredText->SetLabel(wrapped);
				this->text->InvalidateBestSize();
				this->centeredText->InvalidateBestSize();
			}
			this->ApplyBodyAlignment();
			this->Layout();
		}

		void ApplyBodyAlignment() {
			const bool center = (this->side == StudyCardSide::Front || this->side == StudyCardSide::Back)
				&& this->raw.length() < 40;
			this->textSizer->Show(this->text, !center);
			this->textSizer->Show(this->centeredText, center);
		}

		void OnSize(wxSizeEvent& event) {
			if (std::abs(this->visualScale - 1.0) > 0.001) {
				event.Skip();
				return;
			}
			if (!this->wrapping) {
				this->wrapping = true;
				this->Rewrap();
				this->wrapping = false;
			}
			event.Skip();
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}
			const Theme& theme = Theme::Get();
			if (std::abs(this->visualScale - 1.0) > 0.001) {
				theme.FillCanvas(gc, this, size);
				wxGraphicsContext* g = gc.GetGraphicsContext();
				if (g == nullptr) {
					return;
				}
				const int pageWidth = std::max(1, static_cast<int>(size.GetWidth() / this->visualScale));
				const int pageHeight = std::max(1, static_cast<int>(size.GetHeight() / this->visualScale));
				g->PushState();
				g->Scale(this->visualScale, this->visualScale);
				this->DrawScaled(gc, pageWidth, pageHeight);
				g->PopState();
				return;
			}
			const wxColour fill = this->FillColour();
			theme.FillCanvas(gc, this, size);
			theme.DrawRounded(
				gc,
				wxRect(0, 0, size.GetWidth() - 1, size.GetHeight() - 1),
				theme.Dip(this, theme.radius.md),
				fill,
				fill
			);
		}
};

StudyPanel::StudyPanel(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  startingCount(0),
	  answerVisible(false),
	  pageTimer(this),
	  cardHost(nullptr),
	  pageElapsed(-1),
	  spreading(false),
	  guideReadyForBack(false) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);

	this->header = new wxStaticText(this, wxID_ANY, "Study");
	wxFont titleFont = this->header->GetFont();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	this->header->SetFont(titleFont);
	this->header->SetForegroundColour(theme.color.label);
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->cardHost = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->cardHost->SetBackgroundColour(theme.color.window);
	this->backCard = new StudyCard(this->cardHost);
	this->card = new StudyCard(this->cardHost);
	this->backCard->Hide();
	this->rootSizer->Add(this->cardHost, 1, wxEXPAND);

	this->summaryLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	this->rootSizer->Add(this->summaryLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	wxBoxSizer* gradeSizer = new wxBoxSizer(wxHORIZONTAL);
	this->showAnswerButton = new wxButton(this, wxID_ANY, "Show Answer");
	this->rememberedButton = new wxButton(this, wxID_ANY, "Remembered");
	this->forgotButton = new wxButton(this, wxID_ANY, "Forgot");
	gradeSizer->Add(this->showAnswerButton, 0, wxRIGHT, 8);
	gradeSizer->Add(this->rememberedButton, 0, wxRIGHT, 8);
	gradeSizer->Add(this->forgotButton, 0);
	this->rootSizer->Add(gradeSizer, 0, wxALIGN_CENTER | wxALL, 10);

	this->progressLabel = new wxStaticText(this, wxID_ANY, "0 / 0");
	this->rootSizer->Add(this->progressLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 8);

	this->progressBar = new wxGauge(this, wxID_ANY, 1);
	this->progressBar->SetMinSize(wxSize(280, -1));
	this->rootSizer->Add(this->progressBar, 0, wxALIGN_CENTER | wxALL, 8);

	this->todayLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->rootSizer->Add(this->todayLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	this->backButton = new wxButton(this, wxID_ANY, "Back");
	this->rootSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 16);

	this->SetSizer(this->rootSizer);

	this->showAnswerButton->Bind(wxEVT_BUTTON, &StudyPanel::OnShowAnswer, this);
	this->rememberedButton->Bind(wxEVT_BUTTON, &StudyPanel::OnRemembered, this);
	this->forgotButton->Bind(wxEVT_BUTTON, &StudyPanel::OnForgot, this);
	this->backButton->Bind(wxEVT_BUTTON, &StudyPanel::OnBack, this);
	this->Bind(wxEVT_TIMER, &StudyPanel::OnPageTick, this, this->pageTimer.GetId());
	this->cardHost->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
		event.Skip();
		this->CallAfter([this]() { this->ApplySpread(); });
	});

	this->StartStudy();
	this->CallAfter([this]() { this->ApplySpread(); });
}

void StudyPanel::ClosePage() {
	this->pageTimer.Stop();
	this->pageElapsed = -1;
	this->ApplySpread();
}

namespace {
double EaseOut(double t) {
	t = std::clamp(t, 0.0, 1.0);
	return 1.0 - (1.0 - t) * (1.0 - t);
}
}

void StudyPanel::ApplySpread() {
	if (this->spreading || this->card == nullptr || this->backCard == nullptr || this->cardHost == nullptr) {
		return;
	}
	const int row = this->cardHost->GetClientSize().GetWidth();
	const int hostH = this->cardHost->GetClientSize().GetHeight();
	if (row < 2 || hostH < 2) {
		return;
	}
	this->spreading = true;

	constexpr int kSlideMs = 280;
	constexpr int kBackMs = 280;
	double frontSlide = 0;
	double backSlide = 0;
	if (this->pageElapsed >= 0) {
		int t = this->pageElapsed;
		if (t < kSlideMs) {
			frontSlide = EaseOut(t / static_cast<double>(kSlideMs));
		} else {
			t -= kSlideMs;
			frontSlide = 1;
			backSlide = EaseOut(t / static_cast<double>(kBackMs));
		}
	}

	const int gutter = Theme::Get().space.xxl;
	const int cardW = std::max(1, (row / 2) * 2 / 3);
	const int cardH = std::max(1, hostH - 2 * gutter);
	const int slideDistance = cardW / 2 + gutter / 2;
	const int slidLeft = (row - cardW) / 2 - slideDistance;
	const int frontLeft = (row - cardW) / 2 - static_cast<int>(slideDistance * frontSlide);
	const int frontTop = (hostH - cardH) / 2;

	this->card->SetBaseWidth(cardW);
	this->backCard->SetBaseWidth(cardW);
	this->card->Show();
	this->card->SetSize(frontLeft, frontTop, cardW, cardH);
	this->card->Raise();

	if (backSlide <= 0.001) {
		this->backCard->Hide();
	} else {
		const int backDest = slidLeft + cardW + gutter;
		const int backTravel = std::max(0, cardW - gutter);
		const int backLeft = backDest - static_cast<int>((1.0 - backSlide) * backTravel);
		const int backTop = (hostH - cardH) / 2;
		this->backCard->Show();
		this->backCard->SetSize(backLeft, backTop, cardW, cardH);
		this->card->Raise();
	}
	this->cardHost->Refresh();
	this->spreading = false;
}

void StudyPanel::OnPageTick(wxTimerEvent&) {
	constexpr int kPageMs = 280 + 280;
	this->pageElapsed += 16;
	if (this->pageElapsed >= kPageMs) {
		this->pageElapsed = kPageMs;
		this->pageTimer.Stop();
		this->ApplySpread();
		this->rememberedButton->Enable();
		this->forgotButton->Enable();
		this->RefreshGuide();
		return;
	}
	this->ApplySpread();
}

void StudyPanel::StartStudy() {
	auto useCase = wxGetApp().GetInjector().create<StudyDeckUseCase>();
	StudyDeckRequest request;
	request.deckId = this->deckId;
	StudyDeckResponse response = useCase.Execute(request);

	this->deckName = response.deckName;
	this->dueCards = response.dueCards;
	this->startingCount = static_cast<int>(this->dueCards.size());
	this->header->SetLabel(this->deckName.empty() ? "Study" : wxString(this->deckName));
	this->ShowCurrentCard();
}

void StudyPanel::UpdateProgress() {
	const int remaining = static_cast<int>(this->dueCards.size());
	this->progressLabel->SetLabel(wxString::Format("%d / %d", remaining, this->startingCount));
	this->progressBar->SetRange(this->startingCount < 1 ? 1 : this->startingCount);
	this->progressBar->SetValue(this->startingCount - remaining);
}

void StudyPanel::ShowNothingDue() {
	this->ClosePage();
	this->guideReadyForBack = true;
	this->card->ShowNotice("Nothing due today");
	this->summaryLabel->Hide();
	this->showAnswerButton->Hide();
	this->rememberedButton->Hide();
	this->forgotButton->Hide();
	this->progressLabel->Hide();
	this->progressBar->Hide();
	this->todayLabel->Hide();
	this->Layout();
	this->RefreshGuide();
}

void StudyPanel::ShowSummary(const ReviewCardResponse& lastReview) {
	this->ClosePage();
	this->card->ShowSummary(lastReview.cardsReviewed, lastReview.cardsCorrect);
	this->summaryLabel->Hide();
	this->showAnswerButton->Hide();
	this->rememberedButton->Hide();
	this->forgotButton->Hide();
	this->UpdateProgress();
	this->todayLabel->SetLabel(wxString::Format(
		"Today: %d reviewed, %s retention",
		lastReview.cardsReviewed,
		FormatPercent(lastReview.retentionRate)
	));
	this->Layout();
	this->RefreshGuide();
}

void StudyPanel::ShowCurrentCard() {
	if (this->dueCards.empty()) {
		if (this->startingCount == 0) {
			this->ShowNothingDue();
		}
		return;
	}

	this->answerVisible = false;
	this->ClosePage();
	this->card->SetText(wxString(this->dueCards.front().front), StudyCardSide::Front);
	this->summaryLabel->Hide();
	this->showAnswerButton->Show();
	this->showAnswerButton->Enable();
	this->rememberedButton->Show();
	this->forgotButton->Show();
	this->rememberedButton->Disable();
	this->forgotButton->Disable();
	this->progressLabel->Show();
	this->progressBar->Show();
	this->todayLabel->Show();
	this->UpdateProgress();
	this->Layout();
	this->RefreshGuide();
}

void StudyPanel::Grade(bool remembered) {
	if (this->dueCards.empty() || !this->answerVisible) {
		return;
	}

	ReviewCardRequest request;
	request.deckId = this->deckId;
	request.cardId = this->dueCards.front().cardId;
	request.remembered = remembered;

	auto useCase = wxGetApp().GetInjector().create<ReviewCardUseCase>();
	ReviewCardResponse response = useCase.Execute(request);
	if (!response.success) {
		ShowCenteredMessage(this, "Could not save this review.", "Study Deck", wxOK | wxICON_ERROR);
		return;
	}

	this->todayLabel->SetLabel(wxString::Format(
		"Today: %d reviewed, %s retention",
		response.cardsReviewed,
		FormatPercent(response.retentionRate)
	));
	this->dueCards.erase(this->dueCards.begin());
	this->guideReadyForBack = true;
	GuideHighlight::SetBorder(this->showAnswerButton, false);
	GuideHighlight::SetBorder(this->rememberedButton, false);
	GuideHighlight::SetBorder(this->forgotButton, false);
	if (this->dueCards.empty()) {
		this->ShowSummary(response);
		return;
	}
	this->ShowCurrentCard();
}

void StudyPanel::OnShowAnswer(wxCommandEvent&) {
	if (this->dueCards.empty()) {
		return;
	}
	this->answerVisible = true;
	this->showAnswerButton->Disable();
	GuideHighlight::SetBorder(this->showAnswerButton, false);
	this->backCard->SetText(wxString(this->dueCards.front().back), StudyCardSide::Back);
	this->pageElapsed = 0;
	this->pageTimer.Start(16);
	this->ApplySpread();
}

void StudyPanel::OnRemembered(wxCommandEvent&) {
	this->Grade(true);
}

void StudyPanel::OnForgot(wxCommandEvent&) {
	this->Grade(false);
}

void StudyPanel::OnBack(wxCommandEvent&) {
	auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this));
	if (frame == nullptr) {
		return;
	}
	const bool wish = this->guideReadyForBack && this->CompleteGuide();
	frame->ShowCardList();
	if (wish) {
		ShowCenteredMessage(frame, "Happy learning! Your deck is ready whenever you are.", "Merken", wxOK | wxICON_INFORMATION);
	}
}

void StudyPanel::RefreshGuide() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = useCase.Execute();
	if (!profile.ok || profile.guideFinished) {
		GuideHighlight::SetBorder(this->showAnswerButton, false);
		GuideHighlight::SetBorder(this->rememberedButton, false);
		GuideHighlight::SetBorder(this->forgotButton, false);
		GuideHighlight::SetBorder(this->backButton, false);
		return;
	}
	if (this->guideReadyForBack) {
		GuideHighlight::SetBorder(this->showAnswerButton, false);
		GuideHighlight::SetBorder(this->rememberedButton, false);
		GuideHighlight::SetBorder(this->forgotButton, false);
		GuideHighlight::Announce(this->backButton, GuidePrompt::Back, "Whenever you like, let's head back to your decks.");
		return;
	}
	GuideHighlight::SetBorder(this->backButton, false);
	if (!this->showAnswerButton->IsShown()) {
		return;
	}
	if (!this->answerVisible) {
		GuideHighlight::SetBorder(this->rememberedButton, false);
		GuideHighlight::SetBorder(this->forgotButton, false);
		GuideHighlight::Announce(this->showAnswerButton, GuidePrompt::ShowAnswer, "When you're ready, let's reveal the back.");
		return;
	}
	GuideHighlight::SetBorder(this->showAnswerButton, false);
	GuideHighlight::Announce(
		this->rememberedButton,
		GuidePrompt::Grade,
		"If it stuck, mark it Remembered. If not, Forgot is just fine.",
		this->forgotButton
	);
}

bool StudyPanel::CompleteGuide() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse profile = useCase.Execute();
	if (!profile.ok || profile.guideFinished) {
		return false;
	}
	auto finish = wxGetApp().GetInjector().create<FinishGuideUseCase>();
	if (!finish.Execute()) {
		return false;
	}
	GuideHighlight::SetBorder(this->showAnswerButton, false);
	GuideHighlight::SetBorder(this->rememberedButton, false);
	GuideHighlight::SetBorder(this->forgotButton, false);
	GuideHighlight::SetBorder(this->backButton, false);
	return true;
}
