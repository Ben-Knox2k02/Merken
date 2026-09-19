#ifndef DIALOG_H
#define DIALOG_H

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

class DeckDialog : public wxDialog {
	public:
		DeckDialog(wxWindow* parent);
	
		wxBoxSizer* rootSizer;
	
		wxTextCtrl* nameCtrl;
		wxTextCtrl* descriptionCtrl;
		
};

#endif
