#ifndef EVENT_PANEL_H
#define EVENT_PANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/dcbuffer.h>
#include <thread>
#include <memory>
#include "app.h"
#include "mainframe.h"
#include "theme.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_response.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"

class EventPanel : public wxPanel {
	public:
		explicit EventPanel(wxWindow* parent);
		~EventPanel() override;

		//wxPanel* topPanel;
		//wxPanel* midPanel;
		//wxPanel* bottomPanel;
		
		wxBoxSizer* rootSizer;
		//wxBoxSizer* topSizer;
		//wxBoxSizer* midSizer;
		//wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		
		wxListCtrl* eventList;
		
		wxButton* addEventButton;
		wxButton* editEventButton;
		wxButton* deleteEventButton;

		void LoadEvents();
		void ShowEvents(const GetEventsResponse& response);
		
		void OnPaint(wxPaintEvent& event);
		void OnAddEvent(wxCommandEvent& event);
		void OnEditEvent(wxCommandEvent& event);
		void OnDeleteEvent(wxCommandEvent& event);

	private:
		std::shared_ptr<bool> alive;
};

#endif
