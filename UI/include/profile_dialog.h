#ifndef PROFILE_DIALOG_H
#define PROFILE_DIALOG_H

#include <wx/wx.h>
#include <optional>
#include <vector>
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_response.h"

class ProfileDialog : public wxDialog {
	public:
		ProfileDialog(wxWindow* parent, const GetUserProfileResponse& profile);

		wxTextCtrl* nameCtrl;
		wxTextCtrl* noteCtrl;
		wxChoice* goalChoice;
		wxChoice* themeChoice;

		void OnSave(wxCommandEvent& event);

	private:
		std::vector<int> goalValues;
		bool keptUsesAiStudy;

		std::optional<int> SelectedGoal() const;
};

#endif
