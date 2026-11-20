#include "progress_panel.h"
#include "chevron_button.h"
#include "mainframe.h"
#include "theme.h"
#include "app.h"
#include <wx/statline.h>
#include "../../Application/UseCases/Progress/GetTodaysProgress/get_todays_progress_usecase.h"
#include "../../Application/UseCases/Progress/GetProgressHistory/get_progress_history_usecase.h"
#include <wx/datetime.h>
#include <algorithm>

wxDECLARE_APP(App);

namespace {
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

HeatmapPanel::HeatmapPanel(wxWindow* parent)
	: wxPanel(parent),
	  maxReviewed(0),
	  cellSize(this->FromDIP(28)),
	  cellGap(this->FromDIP(6)),
	  leftGutter(this->FromDIP(36)),
	  topGutter(this->FromDIP(12)) {
	this->SetBackgroundColour(this->GetParent()->GetBackgroundColour());
	this->SetMinSize(wxSize(
		this->leftGutter + 12 * (this->cellSize + this->cellGap) + this->FromDIP(12),
		this->topGutter + 7 * (this->cellSize + this->cellGap) + this->FromDIP(12)
	));
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
		this->leftGutter + weeks * (this->cellSize + this->cellGap) + this->FromDIP(12),
		this->topGutter + 7 * (this->cellSize + this->cellGap) + this->FromDIP(12)
	));
	this->Refresh();
}

wxRect HeatmapPanel::CellRect(int week, int weekday) const {
	return wxRect(
		this->leftGutter + week * (this->cellSize + this->cellGap),
		this->topGutter + weekday * (this->cellSize + this->cellGap),
		this->cellSize,
		this->cellSize
	);
}

void HeatmapPanel::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	dc.SetFont(this->GetFont());

	const char* labels[] = {"S", "M", "T", "W", "T", "F", "S"};
	for (int weekday = 0; weekday < 7; ++weekday) {
		const wxString label(labels[weekday]);
		const wxSize text = dc.GetTextExtent(label);
		const int x = (this->leftGutter - text.GetWidth()) / 2;
		const int y = this->topGutter + weekday * (this->cellSize + this->cellGap)
			+ (this->cellSize - text.GetHeight()) / 2;
		dc.DrawText(label, x, y);
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
			dc.DrawRoundedRectangle(this->CellRect(week, weekday), this->FromDIP(4));
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

	const Theme& theme = Theme::Get();
	this->header = new wxStaticText(
		this,
		wxID_ANY,
		"Today's Progress",
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTRE_HORIZONTAL
	);
	wxFont titleFont = this->header->GetFont();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	this->header->SetFont(titleFont);
	this->header->SetForegroundColour(theme.color.label);
	const wxSize backSize = ChevronButtonSize(this, wxString(L"\u2039"));
	this->backButton = new ChevronButton(this, wxString(L"\u2039"), backSize);
	const int leftPad = this->FromDIP(theme.size.cardPad);
	const int rightPad = this->FromDIP(theme.size.panelPad);
	wxBoxSizer* titleRow = new wxBoxSizer(wxHORIZONTAL);
	titleRow->Add(this->backButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, leftPad);
	titleRow->Add(this->header, 1, wxALIGN_CENTER);
	titleRow->AddSpacer(leftPad + backSize.GetWidth());
	this->rootSizer->AddSpacer(this->FromDIP(theme.size.panelPad));
	this->rootSizer->Add(titleRow, 0, wxEXPAND);
	this->rootSizer->AddSpacer(this->FromDIP(theme.space.sm));

	wxBoxSizer* body = new wxBoxSizer(wxVERTICAL);
	body->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND);

	this->todayLabel = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	body->Add(this->todayLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	wxStaticText* heatmapHeader = new wxStaticText(this, wxID_ANY, "Previous days");
	heatmapHeader->SetFont(heatmapHeader->GetFont().Bold());
	body->Add(heatmapHeader, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 16);

	this->heatmap = new HeatmapPanel(this);
	body->Add(this->heatmap, 0, wxALIGN_CENTER | wxALL, 12);

	wxBoxSizer* inset = new wxBoxSizer(wxHORIZONTAL);
	inset->AddSpacer(leftPad);
	inset->Add(body, 1, wxEXPAND);
	inset->AddSpacer(rightPad);
	this->rootSizer->Add(inset, 1, wxEXPAND);

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
