#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <wx/wx.h>
#include <thread>

class SettingsDialog : public wxDialog {
	public:
		explicit SettingsDialog(wxWindow* parent);
		~SettingsDialog() override;
		wxBoxSizer* rootSizer;
		wxTextCtrl* aiKeyCtrl;
		wxTextCtrl* calendarKeyCtrl;
		wxStaticText* googleStatusLabel;
		wxButton* googleButton;
		wxButton* okButton;
		wxButton* cancelButton;
		void OnSave(wxCommandEvent& event);
		void OnGoogleButton(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);

	private:
		std::thread signInThread;
		bool signingIn = false;
		bool googleSignedIn = false;
		void RefreshGoogleStatus();
		void SetBusy(bool busy);
		void StartSignIn();
		void SignOut();
		void OnSignInFinished(bool ok);
};

#endif