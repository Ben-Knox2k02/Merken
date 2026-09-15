#ifndef CALENDAR_PANEL_H
#define CALENDAR_PANEL_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <memory>
#include "../../Application/UseCases/Calendar/GetEvents/get_events_response.h"

class CalendarPanel : public wxPanel {
	public:
		explicit CalendarPanel(wxWindow* parent);
		~CalendarPanel() override;

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		wxStaticText* statusLabel;
		wxDataViewCtrl* eventList;
		wxDataViewListStore* eventViewModel;
		wxButton* backButton;

		void LoadEvents();
		void ShowEvents(const GetEventsResponse& response);
		void OnBack(wxCommandEvent& event);

	private:
		std::shared_ptr<bool> alive;
};

#endif
