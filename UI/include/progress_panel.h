#ifndef PROGRESS_PANEL_H
#define PROGRESS_PANEL_H

#include <wx/wx.h>
#include <vector>
#include "../../Application/UseCases/Progress/GetProgressHistory/get_progress_history_response.h"

class HeatmapPanel : public wxPanel {
	public:
		explicit HeatmapPanel(wxWindow* parent);
		void SetDays(const std::vector<DailyProgressResponse>& days);

	private:
		std::vector<DailyProgressResponse> days;
		int maxReviewed;

		void OnPaint(wxPaintEvent& event);
		void OnMotion(wxMouseEvent& event);
		wxRect CellRect(int week, int weekday) const;
};

class ProgressPanel : public wxPanel {
	public:
		ProgressPanel(wxWindow* parent);

		wxBoxSizer* rootSizer;
		wxStaticText* header;
		wxStaticText* todayLabel;
		HeatmapPanel* heatmap;
		wxButton* backButton;

		void LoadProgress();
		void OnBack(wxCommandEvent& event);
};

#endif
