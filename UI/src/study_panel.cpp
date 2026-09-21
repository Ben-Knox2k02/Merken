#include "study_panel.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "theme.h"
#include "../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"
#include <wx/dcgraph.h>
#include <wx/tokenzr.h>

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
			  wrapping(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);

			const Theme& theme = Theme::Get();
			this->caption = new wxStaticText(
				this,
				wxID_ANY,
				"Front",
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL
			);
			this->caption->SetBackgroundStyle(wxBG_STYLE_COLOUR);
			this->caption->SetBackgroundColour(theme.color.card);
			this->caption->SetForegroundColour(theme.color.label);
			wxFont captionFont = this->caption->GetFont();
			captionFont.SetPointSize(captionFont.GetPointSize() + 3);
			this->caption->SetFont(captionFont);

			this->text = new wxStaticText(
				this,
				wxID_ANY,
				wxEmptyString,
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL
			);
			this->text->SetBackgroundStyle(wxBG_STYLE_COLOUR);
			this->text->SetBackgroundColour(theme.color.card);
			this->text->SetForegroundColour(theme.color.label);
			this->text->SetMinSize(wxSize(0, -1));
			wxFont font = this->text->GetFont();
			font.SetPointSize(font.GetPointSize() + 6);
			this->text->SetFont(font);

			const int pad = this->FromDIP(theme.size.cardPad);
			this->sizer = new wxBoxSizer(wxVERTICAL);
			this->sizer->Add(this->caption, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxLEFT | wxRIGHT, pad);
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
			this->side = side;
			this->raw = value;
			this->lastWrap = -1;
			this->ApplyColours();
			this->Rewrap();
			this->Layout();
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
	  answerVisible(false) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);

	this->header = new wxStaticText(this, wxID_ANY, "Study");
	this->header->SetFont(this->header->GetFont().Bold());
	this->header->SetForegroundColour(theme.color.label);
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->card = new StudyCard(this);
	wxBoxSizer* cardRow = new wxBoxSizer(wxHORIZONTAL);
	cardRow->AddStretchSpacer(1);
	cardRow->Add(this->card, 2, wxEXPAND | wxTOP | wxBOTTOM, theme.space.xxl);
	cardRow->AddStretchSpacer(1);
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

	this->StartStudy();
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
	this->card->SetText(wxString(this->dueCards.front().back), StudyCardSide::Back);
	this->showAnswerButton->Disable();
	this->rememberedButton->Enable();
	this->forgotButton->Enable();
	this->Layout();
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
