#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <wx/wx.h>

class SettingsDialog : public wxDialog {
	public:
		explicit SettingsDialog(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxTextCtrl* aiKeyCtrl;
		wxTextCtrl* calendarKeyCtrl;

		void OnSave(wxCommandEvent& event);
};

#endif
