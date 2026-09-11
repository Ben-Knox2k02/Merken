#ifndef DECK_DIALOG_H
#define DECK_DIALOG_H

#include <wx/wx.h>

class DeckDialog : public wxDialog {
	public:
		DeckDialog(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxTextCtrl* nameCtrl;
		wxTextCtrl* descriptionCtrl;
};

#endif
