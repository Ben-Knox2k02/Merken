#ifndef CALENDAR_PANEL_H
#define CALENDAR_PANEL_H

#include "data_structures.h"
#include "dialog.h"
#include <wx/wx.h>
#include <wx/calctrl.h>
#include <wx/dcbuffer.h>

class CalendarPanel : public wxPanel {
	public:
		CalendarPanel(wxWindow* parent);
		
		wxPanel* topPanel;
		wxPanel* midPanel;
		wxPanel* bottomPanel;
	
		wxBoxSizer* rootSizer;
		wxBoxSizer* topSizer;
		wxBoxSizer* midSizer;
		wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		
		wxCalendarCtrl* calendar;
		
		wxButton* createEventButton;
		wxButton* editEventButton;
		wxButton* deleteEventButton;
		
		void OnPaint(wxPaintEvent& event);
		void OnCreateEvent(wxCommandEvent& event);
		void OnEditEvent(wxCommandEvent& event);
		void OnDeleteEvent(wxCommandEvent& event);
};

#endif
