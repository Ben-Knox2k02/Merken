#ifndef STUDY_PANEL_H
#define STUDY_PANEL_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include "../../Application/UseCases/Deck/GetCards/card_response.h"
#include "../../Application/UseCases/Deck/ReviewCard/review_card_response.h"

class StudyCard;

class StudyPanel : public wxPanel {
	public:
		StudyPanel(wxWindow* parent, int deckId);

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		StudyCard* card;
		wxStaticText* progressLabel;
		wxGauge* progressBar;
		wxStaticText* todayLabel;
		wxStaticText* summaryLabel;
		wxButton* showAnswerButton;
		wxButton* rememberedButton;
		wxButton* forgotButton;
		wxButton* backButton;

		void StartStudy();
		void ShowCurrentCard();
		void ShowNothingDue();
		void ShowSummary(const ReviewCardResponse& lastReview);
		void Grade(bool remembered);
		void UpdateProgress();

		void OnShowAnswer(wxCommandEvent& event);
		void OnRemembered(wxCommandEvent& event);
		void OnForgot(wxCommandEvent& event);
		void OnBack(wxCommandEvent& event);

	private:
		int deckId;
		std::string deckName;
		std::vector<CardResponse> dueCards;
		int startingCount;
		bool answerVisible;
};

#endif
