#ifndef CALENDAR_PANEL_H
#define CALENDAR_PANEL_H

#include "app.h"
#include "mainframe.h"
#include "theme.h"
#include <wx/calctrl.h>
#include <wx/datetime.h>
#include <wx/dcbuffer.h>
#include <memory>
#include "../../Application/UseCases/Calendar/GetEvents/get_events_response.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"

class CalendarPanel : public wxPanel {
	public:
		explicit CalendarPanel(wxWindow* parent);
		~CalendarPanel() override;
		
		wxPanel* topPanel;
		wxPanel* midPanel;
		wxPanel* bottomPanel;
	
		wxBoxSizer* rootSizer;
		wxBoxSizer* topSizer;
		wxBoxSizer* midSizer;
		wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		
		wxCalendarCtrl* calendar;
		
		wxButton* backButton;
		
		void OnPaint(wxPaintEvent& event);
		void OnDateSelected(wxCalendarEvent& event);
		void OnBack(wxCommandEvent& event);

	private:
		std::shared_ptr<bool> alive;
};

#endif
