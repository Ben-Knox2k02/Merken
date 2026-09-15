#include "study_panel.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"

wxDECLARE_APP(App);

namespace {
wxString FormatPercent(double rate) {
	return wxString::Format("%.0f%%", rate * 100.0);
}
}

StudyPanel::StudyPanel(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  startingCount(0),
	  answerVisible(false) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Study");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->promptLabel = new wxStaticText(this, wxID_ANY, "Front");
	this->rootSizer->Add(this->promptLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);

	this->cardText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	wxFont cardFont = this->cardText->GetFont();
	cardFont.SetPointSize(cardFont.GetPointSize() + 6);
	this->cardText->SetFont(cardFont);
	this->rootSizer->Add(this->cardText, 1, wxEXPAND | wxALL, 20);

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
	this->promptLabel->Hide();
	this->cardText->SetLabel("Nothing due today");
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
	this->promptLabel->Hide();
	this->cardText->SetLabel("Session complete");
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
	this->promptLabel->SetLabel("Front");
	this->promptLabel->Show();
	this->cardText->SetLabel(wxString(this->dueCards.front().front));
	this->cardText->Wrap(520);
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
	this->promptLabel->SetLabel("Back");
	this->cardText->SetLabel(wxString(this->dueCards.front().back));
	this->cardText->Wrap(520);
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
