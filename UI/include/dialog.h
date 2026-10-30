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
		wxStaticText* dateText;
		wxStaticText* startText;
		wxStaticText* endText;
		wxStaticText* locationText;
		wxStaticText* descriptionText;
		wxStaticText* reminderText;
		
		wxTextCtrl* titleCtrl;
		wxTextCtrl* dateCtrl;
		wxTextCtrl* startCtrl;
		wxTextCtrl* endCtrl;
		wxTextCtrl* locationCtrl;
		wxTextCtrl* descriptionCtrl;
		wxTextCtrl* reminderCtrl;
		
		wxString GetTitle() const;
		wxString GetDate() const;
		wxString GetStart() const;
		wxString GetEnd() const;
		wxString GetLocation() const;
		wxString GetDescription() const;
		wxString GetReminder() const;
};

#endif
