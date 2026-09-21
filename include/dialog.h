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

class EventDialog : public wxDialog {
	public:
		EventDialog(wxWindow* parent);
		
		wxBoxSizer* rootSizer;
		
		wxStaticText* titleText;
		wxStaticText* dateText;
		wxStaticText* locationText;
		wxStaticText* descriptionText;
		wxStaticText* reminderText;
		
		wxTextCtrl* titleCtrl;
		wxTextCtrl* dateCtrl;
		wxTextCtrl* locationCtrl;
		wxTextCtrl* descriptionCtrl;
		wxTextCtrl* reminderCtrl;
		
};

#endif
