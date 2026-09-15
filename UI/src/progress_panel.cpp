#include "progress_panel.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Progress/GetTodaysProgress/get_todays_progress_usecase.h"
#include "../../Application/UseCases/Progress/GetProgressHistory/get_progress_history_usecase.h"
#include <wx/datetime.h>
#include <algorithm>

wxDECLARE_APP(App);

namespace {
const int kCellSize = 14;
const int kCellGap = 3;
const int kLeftGutter = 28;
const int kTopGutter = 8;

wxColour HeatColor(int reviewed, int maxReviewed) {
	if (reviewed <= 0 || maxReviewed <= 0) {
		return wxColour(235, 237, 240);
	}
	const double t = static_cast<double>(reviewed) / maxReviewed;
	if (t < 0.25) { return wxColour(155, 233, 168); }
	if (t < 0.5) { return wxColour(64, 196, 99); }
	if (t < 0.75) { return wxColour(48, 161, 78); }
	return wxColour(33, 110, 57);
}

wxString FormatPercent(double rate) {
	return wxString::Format("%.0f%%", rate * 100.0);
}

int WeekdayIndex(const std::string& iso) {
	wxDateTime date;
	if (!date.ParseFormat(wxString(iso), "%Y-%m-%d") || !date.IsValid()) {
		return 0;
	}
	return static_cast<int>(date.GetWeekDay());
}
}

HeatmapPanel::HeatmapPanel(wxWindow* parent) : wxPanel(parent), maxReviewed(0) {
	this->SetBackgroundColour(this->GetParent()->GetBackgroundColour());
	this->SetMinSize(wxSize(kLeftGutter + 12 * (kCellSize + kCellGap) + 8, kTopGutter + 7 * (kCellSize + kCellGap) + 8));
	this->Bind(wxEVT_PAINT, &HeatmapPanel::OnPaint, this);
	this->Bind(wxEVT_MOTION, &HeatmapPanel::OnMotion, this);
}

void HeatmapPanel::SetDays(const std::vector<DailyProgressResponse>& days) {
	this->days = days;
	this->maxReviewed = 0;
	for (const DailyProgressResponse& day : this->days) {
		this->maxReviewed = std::max(this->maxReviewed, day.cardsReviewed);
	}
	const int pad = this->days.empty() ? 0 : WeekdayIndex(this->days.front().date);
	const int cells = pad + static_cast<int>(this->days.size());
	const int weeks = cells <= 0 ? 12 : (cells + 6) / 7;
	this->SetMinSize(wxSize(
		kLeftGutter + weeks * (kCellSize + kCellGap) + 8,
		kTopGutter + 7 * (kCellSize + kCellGap) + 8
	));
	this->Refresh();
}

wxRect HeatmapPanel::CellRect(int week, int weekday) const {
	return wxRect(
		kLeftGutter + week * (kCellSize + kCellGap),
		kTopGutter + weekday * (kCellSize + kCellGap),
		kCellSize,
		kCellSize
	);
}

void HeatmapPanel::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	dc.SetFont(this->GetFont().Smaller());

	const char* labels[] = {"S", "M", "T", "W", "T", "F", "S"};
	for (int weekday = 0; weekday < 7; ++weekday) {
		dc.DrawText(labels[weekday], 8, kTopGutter + weekday * (kCellSize + kCellGap));
	}

	const int pad = this->days.empty() ? 0 : WeekdayIndex(this->days.front().date);
	const int cells = pad + static_cast<int>(this->days.size());
	const int weeks = cells <= 0 ? 0 : (cells + 6) / 7;
	dc.SetPen(*wxTRANSPARENT_PEN);
	for (int week = 0; week < weeks; ++week) {
		for (int weekday = 0; weekday < 7; ++weekday) {
			const int index = week * 7 + weekday - pad;
			wxColour color(235, 237, 240);
			if (index >= 0 && index < static_cast<int>(this->days.size())) {
				color = HeatColor(this->days[static_cast<size_t>(index)].cardsReviewed, this->maxReviewed);
			} else if (index < 0 || index >= static_cast<int>(this->days.size())) {
				color = wxColour(245, 245, 247);
			}
			dc.SetBrush(wxBrush(color));
			dc.DrawRoundedRectangle(this->CellRect(week, weekday), 2);
		}
	}
}

void HeatmapPanel::OnMotion(wxMouseEvent& event) {
	const int pad = this->days.empty() ? 0 : WeekdayIndex(this->days.front().date);
	const int cells = pad + static_cast<int>(this->days.size());
	const int weeks = cells <= 0 ? 0 : (cells + 6) / 7;
	for (int week = 0; week < weeks; ++week) {
		for (int weekday = 0; weekday < 7; ++weekday) {
			if (!this->CellRect(week, weekday).Contains(event.GetPosition())) {
				continue;
			}
			const int index = week * 7 + weekday - pad;
			if (index < 0 || index >= static_cast<int>(this->days.size())) {
				this->UnsetToolTip();
				return;
			}
			const DailyProgressResponse& day = this->days[static_cast<size_t>(index)];
			this->SetToolTip(wxString::Format("%s: %d cards", wxString(day.date), day.cardsReviewed));
			return;
		}
	}
	this->UnsetToolTip();
}

ProgressPanel::ProgressPanel(wxWindow* parent) : wxPanel(parent) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Today's Progress");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->todayLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	this->rootSizer->Add(this->todayLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	wxStaticText* heatmapHeader = new wxStaticText(this, wxID_ANY, "Previous days");
	heatmapHeader->SetFont(heatmapHeader->GetFont().Bold());
	this->rootSizer->Add(heatmapHeader, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 16);

	this->heatmap = new HeatmapPanel(this);
	this->rootSizer->Add(this->heatmap, 0, wxALIGN_CENTER | wxALL, 12);

	this->backButton = new wxButton(this, wxID_ANY, "Back");
	this->rootSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 16);

	this->SetSizer(this->rootSizer);
	this->backButton->Bind(wxEVT_BUTTON, &ProgressPanel::OnBack, this);
	this->LoadProgress();
}

void ProgressPanel::LoadProgress() {
	auto todayUseCase = wxGetApp().GetInjector().create<GetTodaysProgressUseCase>();
	GetTodaysProgressResponse today = todayUseCase.Execute();
	this->todayLabel->SetLabel(wxString::Format(
		"%s\nCards reviewed: %d\nCards correct: %d\nRetention: %s",
		wxString(today.date),
		today.cardsReviewed,
		today.cardsCorrect,
		FormatPercent(today.retentionRate)
	));

	auto historyUseCase = wxGetApp().GetInjector().create<GetProgressHistoryUseCase>();
	GetProgressHistoryRequest request;
	GetProgressHistoryResponse history = historyUseCase.Execute(request);
	this->heatmap->SetDays(history.days);
	this->Layout();
}

void ProgressPanel::OnBack(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
