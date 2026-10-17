#ifndef DIALOG_H
#define DIALOG_H

#include <wx/wx.h>
#include <wx/dataview.h>

class CardDialog : public wxDialog {
	public:
		explicit CardDialog(wxWindow* parent, const wxString& title = "Card");
		
		wxBoxSizer* rootSizer;
	
		wxTextCtrl* frontCtrl;
		wxTextCtrl* backCtrl;
		wxTextCtrl* tagCtrl;
};

class DeckDialog : public wxDialog {
	public:
		explicit DeckDialog(wxWindow* parent, const wxString& title = "Deck");
	
		wxBoxSizer* rootSizer;
	
		wxTextCtrl* nameCtrl;
		wxTextCtrl* descriptionCtrl;
};

class EventDialog : public wxDialog {
	public:
		EventDialog(wxWindow* parent);
		
		wxBoxSizer* rootSizer;
		
		wxStaticText* titleText;
		wxStaticText* descriptionText;
		wxStaticText* dateText;
		wxStaticText* locationText;
		wxStaticText* reminderText;
		
		wxTextCtrl* titleCtrl;
		wxTextCtrl* descriptionCtrl;
		wxTextCtrl* dateCtrl;
		wxTextCtrl* locationCtrl;
		wxTextCtrl* reminderCtrl;
};

#endif
