#ifndef ONBOARDING_DIALOG_H
#define ONBOARDING_DIALOG_H

#include <wx/wx.h>
#include <optional>
#include <string>

class OnboardingDialog : public wxDialog {
	public:
		explicit OnboardingDialog(wxWindow* parent);

	private:
		enum class Step {
			Welcome,
			Name,
			Goal
		};

		wxPanel* welcomePanel;
		wxPanel* namePanel;
		wxPanel* goalPanel;
		wxTextCtrl* nameCtrl;
		std::string displayName;
		std::optional<int> dailyGoal;
		bool finished;

		void ShowStep(Step step);
		void ApplyChrome(Step step);
		void Finish();
		void OnWelcomeContinue(wxCommandEvent& event);
		void OnNameContinue(wxCommandEvent& event);
		void OnNameSkip(wxCommandEvent& event);
		void OnGoalChosen(wxCommandEvent& event);
		void OnGoalSkip(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
};

#endif
