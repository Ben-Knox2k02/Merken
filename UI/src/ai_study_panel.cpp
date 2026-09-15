#include "ai_study_panel.h"
#include "centered_message.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/AI/AiStudy/ai_study_usecase.h"
#include "../../Application/UseCases/AI/RecordAiStudyGrade/record_ai_study_grade_usecase.h"
#include <thread>

wxDECLARE_APP(App);

namespace {
wxString FormatPercent(double rate) {
	return wxString::Format("%.0f%%", rate * 100.0);
}

wxString TypeCaption(AiQuestionTypeResponse type) {
	return type == AiQuestionTypeResponse::FillIn ? "Fill in" : "Sentence";
}
}

AiStudyPanel::AiStudyPanel(wxWindow* parent, int deckId)
	: wxPanel(parent),
	  deckId(deckId),
	  currentIndex(0),
	  alive(std::make_shared<bool>(true)) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "AI Study");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->statusLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->rootSizer->Add(this->statusLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	this->typeLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->rootSizer->Add(this->typeLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 16);

	this->questionText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	wxFont questionFont = this->questionText->GetFont();
	questionFont.SetPointSize(questionFont.GetPointSize() + 4);
	this->questionText->SetFont(questionFont);
	this->rootSizer->Add(this->questionText, 1, wxEXPAND | wxALL, 20);

	this->answerText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	this->rootSizer->Add(this->answerText, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 8);

	this->scoreLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	this->rootSizer->Add(this->scoreLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 8);

	wxBoxSizer* gradeSizer = new wxBoxSizer(wxHORIZONTAL);
	this->showAnswerButton = new wxButton(this, wxID_ANY, "Show answer");
	this->gotItButton = new wxButton(this, wxID_ANY, "Got it");
	this->missedItButton = new wxButton(this, wxID_ANY, "Missed it");
	gradeSizer->Add(this->showAnswerButton, 0, wxRIGHT, 8);
	gradeSizer->Add(this->gotItButton, 0, wxRIGHT, 8);
	gradeSizer->Add(this->missedItButton, 0);
	this->rootSizer->Add(gradeSizer, 0, wxALIGN_CENTER | wxALL, 10);

	this->startAgainButton = new wxButton(this, wxID_ANY, "Start test again");
	this->rootSizer->Add(this->startAgainButton, 0, wxALIGN_CENTER | wxALL, 4);

	this->backButton = new wxButton(this, wxID_ANY, "Back");
	this->rootSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 16);

	this->SetSizer(this->rootSizer);

	this->showAnswerButton->Bind(wxEVT_BUTTON, &AiStudyPanel::OnShowAnswer, this);
	this->gotItButton->Bind(wxEVT_BUTTON, &AiStudyPanel::OnGotIt, this);
	this->missedItButton->Bind(wxEVT_BUTTON, &AiStudyPanel::OnMissedIt, this);
	this->startAgainButton->Bind(wxEVT_BUTTON, &AiStudyPanel::OnStartAgain, this);
	this->backButton->Bind(wxEVT_BUTTON, &AiStudyPanel::OnBack, this);

	this->StartTest();
}

AiStudyPanel::~AiStudyPanel() {
	*this->alive = false;
}

void AiStudyPanel::StartTest() {
	this->questions.clear();
	this->currentIndex = 0;
	this->header->SetLabel("AI Study");
	this->statusLabel->SetLabel("Loading questions…");
	this->typeLabel->Hide();
	this->questionText->SetLabel(wxEmptyString);
	this->answerText->Hide();
	this->scoreLabel->Hide();
	this->showAnswerButton->Hide();
	this->gotItButton->Hide();
	this->missedItButton->Hide();
	this->startAgainButton->Hide();
	this->Layout();

	auto useCase = wxGetApp().GetInjector().create<AiStudyUseCase>();
	AiStudyRequest request;
	request.deckId = this->deckId;
	auto alive = this->alive;
	std::thread([this, alive, useCase, request]() mutable {
		AiStudyResponse response = useCase.Execute(request);
		if (wxTheApp == nullptr) { return; }
		wxTheApp->CallAfter([this, alive, response]() {
			if (!*alive) { return; }
			this->OnLoaded(response);
		});
	}).detach();
}

void AiStudyPanel::OnLoaded(const AiStudyResponse& response) {
	this->deckName = response.deckName;
	if (!this->deckName.empty()) {
		this->header->SetLabel(wxString(this->deckName));
	}

	if (!response.success) {
		this->ShowError("Could not start AI study. Check that the deck has cards and a Gemini API key is saved.");
		return;
	}

	this->questions = response.questions;
	this->currentIndex = 0;
	this->ShowCurrentQuestion();
}

void AiStudyPanel::ShowError(const wxString& message) {
	this->statusLabel->SetLabel(message);
	this->statusLabel->Wrap(480);
	this->typeLabel->Hide();
	this->questionText->SetLabel(wxEmptyString);
	this->answerText->Hide();
	this->scoreLabel->Hide();
	this->showAnswerButton->Hide();
	this->gotItButton->Hide();
	this->missedItButton->Hide();
	this->startAgainButton->Show();
	this->Layout();
}

void AiStudyPanel::ShowCurrentQuestion() {
	if (this->currentIndex >= this->questions.size()) {
		return;
	}

	const AiQuestionResponse& question = this->questions[this->currentIndex];
	this->statusLabel->SetLabel(wxString::Format(
		"%d / %d",
		static_cast<int>(this->currentIndex) + 1,
		static_cast<int>(this->questions.size())
	));
	this->typeLabel->SetLabel(TypeCaption(question.type));
	this->typeLabel->Show();
	this->questionText->SetLabel(wxString(question.text));
	this->questionText->Wrap(520);
	this->answerText->Hide();
	this->scoreLabel->Show();
	this->showAnswerButton->Show();
	this->showAnswerButton->Enable();
	this->gotItButton->Show();
	this->missedItButton->Show();
	this->gotItButton->Disable();
	this->missedItButton->Disable();
	this->startAgainButton->Hide();
	this->Layout();
}

void AiStudyPanel::ShowSummary(const RecordAiStudyGradeResponse& lastGrade) {
	this->statusLabel->SetLabel("Test complete");
	this->typeLabel->Hide();
	this->questionText->SetLabel(wxEmptyString);
	this->answerText->Hide();
	this->scoreLabel->SetLabel(wxString::Format(
		"Correct: %d\nWrong: %d\n%s",
		lastGrade.correct,
		lastGrade.wrong,
		FormatPercent(lastGrade.percentage)
	));
	this->scoreLabel->Show();
	this->showAnswerButton->Hide();
	this->gotItButton->Hide();
	this->missedItButton->Hide();
	this->startAgainButton->Show();
	this->Layout();
}

void AiStudyPanel::Grade(bool gotIt) {
	RecordAiStudyGradeRequest request;
	request.gotIt = gotIt;

	auto useCase = wxGetApp().GetInjector().create<RecordAiStudyGradeUseCase>();
	RecordAiStudyGradeResponse response = useCase.Execute(request);
	if (!response.success) {
		ShowCenteredMessage(this, "Could not record that grade.", "AI Study", wxOK | wxICON_ERROR);
		return;
	}

	this->scoreLabel->SetLabel(wxString::Format(
		"Correct: %d   Wrong: %d   %s",
		response.correct,
		response.wrong,
		FormatPercent(response.percentage)
	));
	this->scoreLabel->Show();

	if (response.complete) {
		this->ShowSummary(response);
		return;
	}

	this->currentIndex += 1;
	this->ShowCurrentQuestion();
}

void AiStudyPanel::OnShowAnswer(wxCommandEvent&) {
	if (this->currentIndex >= this->questions.size()) {
		return;
	}
	this->answerText->SetLabel(wxString(this->questions[this->currentIndex].back));
	this->answerText->Wrap(520);
	this->answerText->Show();
	this->showAnswerButton->Disable();
	this->gotItButton->Enable();
	this->missedItButton->Enable();
	this->Layout();
}

void AiStudyPanel::OnGotIt(wxCommandEvent&) {
	this->Grade(true);
}

void AiStudyPanel::OnMissedIt(wxCommandEvent&) {
	this->Grade(false);
}

void AiStudyPanel::OnStartAgain(wxCommandEvent&) {
	this->StartTest();
}

void AiStudyPanel::OnBack(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
