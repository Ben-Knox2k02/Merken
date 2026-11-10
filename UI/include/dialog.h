#ifndef DIALOG_H
#define DIALOG_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/spinctrl.h>
#include "../../DomainModels/date.h"

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
		explicit EventDialog(wxWindow* parent, const wxString& title = "Add Event");

		void SetEventTitle(const std::string& title);
		std::string GetEventTitle() const;
		void SetEventDate(const Date& date);
		Date GetEventDate() const;
		void SetStartTime(const std::string& clock);
		std::string GetStartTime() const;
		void SetEndTime(const std::string& clock);
		std::string GetEndTime() const;
		void SetLocation(const std::string& location);
		std::string GetLocation() const;
		void SetDescription(const std::string& description);
		std::string GetDescription() const;
		void SetReminderMinutes(int minutes);
		int GetReminderMinutes() const;

	private:
		wxTextCtrl* titleCtrl;
		wxChoice* monthCtrl;
		wxChoice* dayCtrl;
		wxSpinCtrl* yearCtrl;
		wxChoice* startHourCtrl;
		wxChoice* startMinuteCtrl;
		wxChoice* endHourCtrl;
		wxChoice* endMinuteCtrl;
		wxTextCtrl* locationCtrl;
		wxTextCtrl* descriptionCtrl;
		wxChoice* reminderCtrl;

		void RefreshDays();
		void OnOk(wxCommandEvent& event);
};

#endif
