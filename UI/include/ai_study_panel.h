#ifndef AI_STUDY_PANEL_H
#define AI_STUDY_PANEL_H

#include <wx/wx.h>
#include <memory>
#include <vector>
#include "../../Application/UseCases/AI/AiStudy/ai_study_response.h"
#include "../../Application/UseCases/AI/RecordAiStudyGrade/record_ai_study_grade_response.h"

class AiStudyPanel : public wxPanel {
	public:
		AiStudyPanel(wxWindow* parent, int deckId);
		~AiStudyPanel() override;

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		wxStaticText* statusLabel;
		wxStaticText* typeLabel;
		wxStaticText* questionText;
		wxStaticText* answerText;
		wxStaticText* scoreLabel;
		wxButton* showAnswerButton;
		wxButton* gotItButton;
		wxButton* missedItButton;
		wxButton* startAgainButton;
		wxButton* backButton;

		void StartTest();
		void OnLoaded(const AiStudyResponse& response);
		void ShowCurrentQuestion();
		void ShowSummary(const RecordAiStudyGradeResponse& lastGrade);
		void ShowError(const wxString& message);
		void Grade(bool gotIt);

		void OnShowAnswer(wxCommandEvent& event);
		void OnGotIt(wxCommandEvent& event);
		void OnMissedIt(wxCommandEvent& event);
		void OnStartAgain(wxCommandEvent& event);
		void OnBack(wxCommandEvent& event);

	private:
		int deckId;
		std::string deckName;
		std::vector<AiQuestionResponse> questions;
		size_t currentIndex;
		std::shared_ptr<bool> alive;
};

#endif
