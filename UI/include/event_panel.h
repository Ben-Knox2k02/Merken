#ifndef EVENT_PANEL_H
#define EVENT_PANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <memory>
#include "../../Application/UseCases/Calendar/GetEvents/get_events_response.h"

class EventPanel : public wxPanel {
	public:
		explicit EventPanel(wxWindow* parent);
		~EventPanel() override;

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		wxStaticText* statusLabel;
		wxListCtrl* eventList;
		wxButton* backButton;

		void LoadEvents();
		void ShowEvents(const GetEventsResponse& response);
		void OnPaint(wxPaintEvent& event);
		void OnBack(wxCommandEvent& event);

	private:
		std::shared_ptr<bool> alive;
};

#endif
