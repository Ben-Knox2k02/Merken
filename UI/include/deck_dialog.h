#ifndef DECK_DIALOG_H
#define DECK_DIALOG_H

#include <wx/wx.h>

class DeckDialog : public wxDialog {
	public:
		explicit DeckDialog(wxWindow* parent, const wxString& title = "Deck");

		wxBoxSizer* rootSizer;
		wxTextCtrl* nameCtrl;
		wxTextCtrl* descriptionCtrl;
};

#endif
