#ifndef TODAYS_EVENTS_PANEL_H
#define TODAYS_EVENTS_PANEL_H

#include <wx/wx.h>
#include <memory>
#include <vector>
#include "../../Application/UseCases/Calendar/GetEvents/event_response.h"

class TodaysEventsPanel : public wxPanel {
	public:
		explicit TodaysEventsPanel(wxWindow* parent);

		~TodaysEventsPanel() override;

		void LoadEvents();
		void ApplyTheme();

	private:
		wxBoxSizer* rootSizer;
		wxStaticText* heading;
		wxScrolledWindow* scroller;
		wxBoxSizer* listSizer;
		std::shared_ptr<bool> alive;
		int generation;

		static constexpr int kMinWidth = 320 * 2 / 3;

		void ShowEvents(const std::vector<EventResponse>& events);
		void ShowMessage(const wxString& text);
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
};

#endif
