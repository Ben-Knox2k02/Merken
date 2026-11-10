#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <wx/wx.h>
#include <memory>
#include <thread>

class SettingsDialog : public wxDialog {
	public:
		explicit SettingsDialog(wxWindow* parent);
		~SettingsDialog() override;

		wxBoxSizer* rootSizer;
		wxTextCtrl* aiKeyCtrl;
		wxTextCtrl* calendarKeyCtrl;
		wxStaticText* statusLabel;
		wxButton* googleButton;
		wxButton* okButton;
		wxButton* cancelButton;

		void OnSave(wxCommandEvent& event);

	private:
		std::shared_ptr<bool> alive;
		std::thread worker;
		bool signingIn = false;
		bool googleSignedIn = false;

		void OnGoogle(wxCommandEvent& event);
		void OnAuthFinished(bool failed, bool fromSignOut);
		void OnClose(wxCloseEvent& event);
		void ApplySignInState(bool signedIn);
		void JoinWorker();
};

#endif
