#include "study_panel.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "theme.h"
#include "../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"
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

int LabelHeight(wxWindow* window, const wxString& wrapped) {
	const int lineHeight = window->GetTextExtent("Ag").GetHeight();
	if (wrapped.empty()) {
		return lineHeight;
	}
	int lines = 1;
	for (size_t i = 0; i < wrapped.length(); ++i) {
		if (wrapped[i] == '\n') {
			++lines;
		}
	}
	return lines * lineHeight;
}
}

enum class StudyCardSide {
	Neutral,
	Front,
	Back
};

class StudyCard : public wxPanel {
	public:
		explicit StudyCard(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  side(StudyCardSide::Neutral),
			  lastWrap(-1),
			  wrapping(false),
			  hinge(1),
			  hingeFullWidth(0) {
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

			this->text = new wxStaticText(
				this,
				wxID_ANY,
				wxEmptyString,
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_LEFT
			);
			this->text->SetBackgroundStyle(wxBG_STYLE_COLOUR);
			this->text->SetBackgroundColour(theme.color.card);
			this->text->SetForegroundColour(theme.color.label);
			this->text->SetMinSize(wxSize(0, -1));
			wxFont font = this->text->GetFont();
			font.SetPointSize(font.GetPointSize() + 6);
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			this->text->SetFont(font);

			const int pad = this->FromDIP(theme.size.cardPad);
			this->sizer = new wxBoxSizer(wxVERTICAL);
			this->sizer->Add(this->caption, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, pad);
			this->sizer->AddStretchSpacer(1);
			this->sizer->Add(this->text, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
			this->sizer->AddStretchSpacer(1);
			this->sizer->Show(this->caption, false);
			this->SetSizer(this->sizer);

			this->Bind(wxEVT_PAINT, &StudyCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &StudyCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetText(const wxString& value, StudyCardSide side = StudyCardSide::Neutral) {
			this->hinge = 1;
			this->side = side;
			this->raw = value;
			this->lastWrap = -1;
			this->ApplyColours();
			this->Rewrap();
			this->ShowFace();
			this->Refresh();
		}

		void SetHinge(double reveal, int fullWidth) {
			this->hinge = std::clamp(reveal, 0.0, 1.0);
			this->hingeFullWidth = std::max(fullWidth, 1);
			if (this->hinge >= 0.999) {
				this->hinge = 1;
				this->lastWrap = -1;
				this->ShowFace();
				this->Rewrap();
			} else {
				this->HideFace();
			}
			this->Refresh();
		}

	private:
		wxBoxSizer* sizer;
		wxStaticText* caption;
		wxStaticText* text;
		wxString raw;
		StudyCardSide side;
		int lastWrap;
		bool wrapping;
		double hinge;
		int hingeFullWidth;

		void HideFace() {
			this->sizer->Show(this->caption, false);
			this->sizer->Show(this->text, false);
		}

		void ShowFace() {
			this->sizer->Show(this->text, true);
			this->sizer->Show(this->caption, this->side != StudyCardSide::Neutral);
			this->Layout();
		}

		void DrawFace(wxGCDC& gc, int pageWidth, int pageHeight) {
			const Theme& theme = Theme::Get();
			const wxColour fill = this->FillColour();
			theme.DrawRounded(
				gc,
				wxRect(0, 0, pageWidth - 1, pageHeight - 1),
				theme.Dip(this, theme.radius.md),
				fill,
				fill
			);
		}

		void DrawHinge(wxGCDC& gc, const wxSize& size) {
			const Theme& theme = Theme::Get();
			theme.FillCanvas(gc, this, size);
			const int pageWidth = this->hingeFullWidth;
			const int pageHeight = size.GetHeight();
			if (pageWidth < 2 || pageHeight < 2) {
				return;
			}
			wxGraphicsContext* g = gc.GetGraphicsContext();
			if (g == nullptr) {
				return;
			}

			const double angle = (1.0 - this->hinge) * 3.141592653589793 / 2.0;
			const double cosine = std::max(0.02, std::cos(angle));
			const double sine = std::sin(angle);
			g->PushState();
			g->Translate(0, pageHeight / 2.0);
			g->Scale(cosine, 1.0 - sine * 0.14);
			g->Translate(0, -pageHeight / 2.0);
			this->DrawFace(gc, pageWidth, pageHeight);
			const int shade = static_cast<int>(sine * 150.0);
			if (shade > 0) {
				g->SetPen(*wxTRANSPARENT_PEN);
				g->SetBrush(wxBrush(wxColour(0, 0, 0, shade)));
				g->DrawRectangle(0, 0, pageWidth, pageHeight);
			}
			g->PopState();
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
			this->text->SetBackgroundColour(fill);
			this->text->SetForegroundColour(ink);
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
			const int pad = this->FromDIP(Theme::Get().size.cardPad);
			int wrapWidth = this->GetClientSize().GetWidth() - 2 * pad;
			const int minWrap = this->FromDIP(Theme::Get().size.minWrap);
			if (wrapWidth < minWrap) {
				if (this->lastWrap != -1) {
					this->text->SetLabel(this->raw);
					this->text->SetMinSize(wxSize(0, -1));
					this->text->InvalidateBestSize();
					this->lastWrap = -1;
				}
				return;
			}
			if (this->lastWrap == wrapWidth) {
				return;
			}
			this->lastWrap = wrapWidth;
			this->ApplyColours();
			const wxString wrapped = WrapToWidth(this->text, this->raw, wrapWidth);
			this->text->SetLabel(wrapped);
			this->text->SetMinSize(wxSize(0, LabelHeight(this->text, wrapped)));
			this->text->InvalidateBestSize();
			this->Layout();
		}

		void OnSize(wxSizeEvent& event) {
			if (this->hinge < 0.999) {
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
			if (this->hinge < 0.999) {
				this->DrawHinge(gc, size);
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
	  pageLead(nullptr),
	  pageGap(nullptr),
	  pageTail(nullptr),
	  pageElapsed(-1),
	  spreading(false) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);

	this->header = new wxStaticText(this, wxID_ANY, "Study");
	this->header->SetFont(this->header->GetFont().Bold());
	this->header->SetForegroundColour(theme.color.label);
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->card = new StudyCard(this);
	this->backCard = new StudyCard(this);
	this->backCard->Hide();
	wxBoxSizer* cardRow = new wxBoxSizer(wxHORIZONTAL);
	this->pageLead = cardRow->AddSpacer(0);
	cardRow->Add(this->card, 0, wxEXPAND | wxTOP | wxBOTTOM, theme.space.xxl);
	this->pageGap = cardRow->AddSpacer(0);
	cardRow->Add(this->backCard, 0, wxEXPAND | wxTOP | wxBOTTOM, theme.space.xxl);
	this->pageTail = cardRow->AddSpacer(0);
	this->rootSizer->Add(cardRow, 1, wxEXPAND);

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
	this->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
		this->ApplySpread();
		event.Skip();
	});

	this->StartStudy();
}

void StudyPanel::ClosePage() {
	this->pageTimer.Stop();
	this->pageElapsed = -1;
	this->ApplySpread();
}

void StudyPanel::ApplySpread() {
	if (this->spreading || this->card == nullptr || this->backCard == nullptr) {
		return;
	}
	const int row = this->GetClientSize().GetWidth();
	if (row < 2) {
		return;
	}
	this->spreading = true;

	constexpr int kSlideMs = 280;
	constexpr int kFlipMs = 520;
	double slide = 0;
	double flip = 0;
	if (this->pageElapsed >= 0) {
		if (this->pageElapsed <= kSlideMs) {
			const double t = this->pageElapsed / static_cast<double>(kSlideMs);
			slide = 1.0 - (1.0 - t) * (1.0 - t);
		} else {
			slide = 1;
			const double t = std::min(1.0, (this->pageElapsed - kSlideMs) / static_cast<double>(kFlipMs));
			flip = t;
		}
	}

	const int gutterFull = Theme::Get().space.xxl;
	const int cardW = std::max(1, (row / 2) * 2 / 3);
	const int centeredLead = std::max(0, (row - cardW) / 2);
	const int slideDistance = cardW / 2 + gutterFull;
	const int lead = std::max(0, centeredLead - static_cast<int>(slideDistance * slide));
	const int gap = static_cast<int>(gutterFull * slide);
	const double angle = (1.0 - flip) * 3.141592653589793 / 2.0;
	const int backW = flip <= 0.001 ? 0 : std::max(1, static_cast<int>(cardW * std::cos(angle)));
	const int tail = std::max(0, row - lead - cardW - gap - backW);

	this->card->SetMinSize(wxSize(cardW, -1));
	this->card->SetMaxSize(wxSize(cardW, -1));
	this->backCard->SetMinSize(wxSize(std::max(backW, 0), -1));
	this->backCard->SetMaxSize(wxSize(std::max(backW, 1), -1));
	if (this->pageLead != nullptr) {
		this->pageLead->AssignSpacer(lead, 0);
	}
	if (this->pageGap != nullptr) {
		this->pageGap->AssignSpacer(gap, 0);
	}
	if (this->pageTail != nullptr) {
		this->pageTail->AssignSpacer(tail, 0);
	}
	if (backW < 2) {
		this->backCard->Hide();
	} else {
		this->backCard->Show();
		this->backCard->SetHinge(flip, cardW);
	}
	this->Layout();
	this->spreading = false;
}

void StudyPanel::OnPageTick(wxTimerEvent&) {
	constexpr int kPageMs = 280 + 520;
	this->pageElapsed += 16;
	if (this->pageElapsed >= kPageMs) {
		this->pageElapsed = kPageMs;
		this->pageTimer.Stop();
		this->ApplySpread();
		this->rememberedButton->Enable();
		this->forgotButton->Enable();
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
	this->card->SetText("Nothing due today");
	this->summaryLabel->Hide();
	this->showAnswerButton->Hide();
	this->rememberedButton->Hide();
	this->forgotButton->Hide();
	this->progressLabel->Hide();
	this->progressBar->Hide();
	this->todayLabel->Hide();
	this->Layout();
}

void StudyPanel::ShowSummary(const ReviewCardResponse& lastReview) {
	this->ClosePage();
	this->card->SetText("Session complete");
	this->summaryLabel->SetLabel(wxString::Format(
		"Cards reviewed: %d\nCards correct: %d\nRetention: %s",
		lastReview.cardsReviewed,
		lastReview.cardsCorrect,
		FormatPercent(lastReview.retentionRate)
	));
	this->summaryLabel->Show();
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
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
