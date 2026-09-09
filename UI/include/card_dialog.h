#ifndef CARD_DIALOG_H
#define CARD_DIALOG_H

#include <wx/wx.h>
#include <wx/dataview.h>

class CardDialog : public wxDialog {
	public:
		CardDialog(wxWindow* parent);
		
		wxBoxSizer* rootSizer;
	
		wxTextCtrl* frontCtrl;
		wxTextCtrl* backCtrl;
		wxTextCtrl* tagCtrl;
};

#endif
