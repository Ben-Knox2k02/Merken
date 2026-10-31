#ifndef DIALOG_H
#define DIALOG_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/choice.h>
#include <wx/timectrl.h>
#include <wx/datectrl.h>
#include <wx/listctrl.h>

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
		wxDatePickerCtrl* dateCtrl;
		wxChoice* startHourChoice;
		wxChoice* startMinuteChoice;
		wxChoice* endHourChoice;
		wxChoice* endMinuteChoice;
		wxTextCtrl* locationCtrl;
		wxTextCtrl* descriptionCtrl;
		wxTextCtrl* reminderCtrl;
		
		wxString GetTitle() const;
		
		wxDateTime GetDate() const;
		wxDateTime GetStart() const;
		wxDateTime GetEnd() const;
		
		wxString GetLocation() const;
		wxString GetDescription() const;
		wxString GetReminder() const;
		
		void SetTitle(const wxString& title);
		
		void SetDate(const wxDateTime& dt);
		void SetStart(const wxDateTime& dt);
		void SetEnd(const wxDateTime& dt);
		
		void SetLocation(const wxString& location);
		void SetDescription(const wxString& description);
		void SetReminder(const wxString& reminder);
};

#endif
