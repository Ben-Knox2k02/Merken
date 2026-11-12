#ifndef CALENDAR_PANEL_H
#define CALENDAR_PANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../../Application/UseCases/Calendar/GetEvents/get_events_response.h"
#include "../../DomainModels/date.h"

// wxCalendarCtrl is in the wxWidgets adv library, which this build does not link.
class MonthCalendar : public wxPanel {
	public:
		explicit MonthCalendar(wxWindow* parent);

		void SetDate(const Date& date);
		const Date& GetDate() const;
		void ShiftMonth(int delta);

	private:
		int visibleYear;
		int visibleMonth;
		Date selected;

		int CellSize() const;
		int ColumnWidth(int column) const;
		int ColumnLeft(int column) const;
		int HeaderHeight() const;
		int LeadDays() const;
		int HitDay(const wxPoint& position) const;
		wxRect DayRect(int row, int column) const;
		void Notify();
		void OnPaint(wxPaintEvent& event);
		void OnClick(wxMouseEvent& event);
};

class CalendarPanel : public wxPanel {
	public:
		explicit CalendarPanel(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		wxPanel* calendarColumn;
		wxPanel* eventColumn;
		wxButton* previousButton;
		wxButton* nextButton;
		wxStaticText* monthLabel;
		MonthCalendar* monthGrid;
		wxButton* addButton;
		wxButton* editButton;
		wxButton* deleteButton;
		wxStaticText* statusLabel;
		wxListCtrl* eventList;
		wxButton* backButton;

		void LoadEvents();
		void OnPaint(wxPaintEvent& event);
		void OnBack(wxCommandEvent& event);
		void OnPrevious(wxCommandEvent& event);
		void OnNext(wxCommandEvent& event);
		void OnDateChanged(wxCommandEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnSelectionChanged(wxListEvent& event);

	private:
		std::vector<EventResponse> dayEvents;
		wxString notice;

		void SyncMonthLabel();
		void ShowDay(const GetEventsResponse& response);
		void ApplyColumnWidths();
		void OnSize(wxSizeEvent& event);
		void UpdateActionButtons();
		int SelectedEventId() const;
		const EventResponse* SelectedEvent() const;
};

#endif
