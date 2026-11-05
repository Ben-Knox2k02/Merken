#include "todays_events_panel.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include <wx/dcgraph.h>
#include <algorithm>
#include <thread>

wxDECLARE_APP(App);

namespace {
bool ParseWhen(const std::string& text, wxDateTime& when) {
	if (text.empty()) {
		return false;
	}
	const wxString value(text);
	return when.ParseISOCombined(value) || when.ParseISODate(value);
}

bool FallsOnToday(const EventResponse& event) {
	const wxDateTime today = wxDateTime::Today();
	wxDateTime dayStart = today;
	dayStart.ResetTime();
	wxDateTime dayEnd = dayStart;
	dayEnd += wxTimeSpan::Day();

	wxDateTime start;
	if (!ParseWhen(event.startTime, start)) {
		wxDateTime end;
		return ParseWhen(event.endTime, end) && end >= dayStart && end < dayEnd;
	}

	const bool startIsDate = event.startTime.find('T') == std::string::npos;
	wxDateTime finish;
	const bool hasEnd = ParseWhen(event.endTime, finish);
	if (!hasEnd) {
		if (!startIsDate) {
			return start >= dayStart && start < dayEnd;
		}
		finish = start;
		finish += wxTimeSpan::Day();
	}

	const bool endIsDate = hasEnd && event.endTime.find('T') == std::string::npos;
	if (startIsDate || endIsDate) {
		return start < dayEnd && finish > dayStart;
	}
	return start < dayEnd && finish >= dayStart;
}

wxString FormatWhen(const EventResponse& event) {
	wxDateTime start;
	wxDateTime end;
	const bool hasStart = ParseWhen(event.startTime, start);
	const bool hasEnd = ParseWhen(event.endTime, end);
	const bool startIsDate = event.startTime.find('T') == std::string::npos;
	if (startIsDate && hasStart) {
		return "All day";
	}
	if (!hasStart) {
		return wxString(event.startTime);
	}
	const wxString startText = start.Format("%H:%M");
	if (!hasEnd) {
		return startText;
	}
	return startText + " - " + end.Format("%H:%M");
}
}

TodaysEventsPanel::TodaysEventsPanel(wxWindow* parent)
	: wxPanel(),
	  alive(std::make_shared<bool>(true)),
	  generation(0) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->SetBackgroundColour(Theme::Get().color.window);
	this->SetMinSize(wxSize(this->FromDIP(kMinWidth), -1));

	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const int pad = this->FromDIP(Theme::Get().size.cardPad);
	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().size.panelPad));
	this->heading = new wxStaticText(this, wxID_ANY, "Today's events");
	wxFont headingFont = this->heading->GetFont();
	headingFont.SetWeight(wxFONTWEIGHT_NORMAL);
	this->heading->SetFont(headingFont);
	this->heading->SetForegroundColour(Theme::Get().color.secondaryLabel);
	this->rootSizer->Add(this->heading, 0, wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, pad);

	this->scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	this->scroller->SetWindowStyleFlag(this->scroller->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
	this->scroller->SetBackgroundColour(Theme::Get().color.window);
	this->scroller->SetScrollRate(0, 16);
	this->scroller->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	this->listSizer = new wxBoxSizer(wxVERTICAL);
	this->scroller->SetSizer(this->listSizer);
	this->rootSizer->Add(this->scroller, 1, wxEXPAND | wxLEFT | wxRIGHT, pad);
	this->rootSizer->AddSpacer(this->FromDIP(Theme::Get().space.md));

	this->SetSizer(this->rootSizer);
	this->Bind(wxEVT_PAINT, &TodaysEventsPanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, &TodaysEventsPanel::OnSize, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});

	this->ShowMessage("Loading events...");
	this->LoadEvents();
}

TodaysEventsPanel::~TodaysEventsPanel() {
	*this->alive = false;
}

void TodaysEventsPanel::LoadEvents() {
	const int ticket = ++this->generation;
	auto useCase = wxGetApp().GetInjector().create<GetEventsUseCase>();
	auto aliveFlag = this->alive;
	std::thread([this, aliveFlag, ticket, useCase]() mutable {
		GetEventsResponse response = useCase.Execute();
		if (wxTheApp == nullptr) {
			return;
		}
		wxTheApp->CallAfter([this, aliveFlag, ticket, response]() {
			if (!*aliveFlag || ticket != this->generation) {
				return;
			}
			std::vector<EventResponse> today;
			for (const EventResponse& event : response.events) {
				if (FallsOnToday(event)) {
					today.push_back(event);
				}
			}
			std::sort(today.begin(), today.end(), [](const EventResponse& left, const EventResponse& right) {
				return left.startTime < right.startTime;
			});
			this->ShowEvents(today);
		});
	}).detach();
}

void TodaysEventsPanel::ShowMessage(const wxString& text) {
	this->listSizer->Clear(true);
	const Theme& theme = Theme::Get();
	wxStaticText* message = new wxStaticText(this->scroller, wxID_ANY, text);
	message->SetForegroundColour(theme.color.secondaryLabel);
	const int pad = this->FromDIP(Theme::Get().space.lg);
	this->listSizer->Add(message, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
	this->scroller->FitInside();
	this->scroller->Layout();
	this->Layout();
}

void TodaysEventsPanel::ShowEvents(const std::vector<EventResponse>& events) {
	if (events.empty()) {
		this->ShowMessage("No events today");
		return;
	}

	this->listSizer->Clear(true);
	const Theme& theme = Theme::Get();
	const int padX = this->FromDIP(Theme::Get().space.lg);
	const int padY = this->FromDIP(Theme::Get().space.sm);
	const int gap = this->FromDIP(Theme::Get().space.xxs);
	for (const EventResponse& event : events) {
		wxPanel* row = new wxPanel(this->scroller, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
		row->SetBackgroundColour(theme.color.window);
		const wxString title = event.title.empty() ? wxString("Untitled") : wxString(event.title);
		wxStaticText* titleLabel = new wxStaticText(
			row,
			wxID_ANY,
			title,
			wxDefaultPosition,
			wxDefaultSize,
			wxST_ELLIPSIZE_END
		);
		titleLabel->SetForegroundColour(theme.color.label);
		wxStaticText* timeLabel = new wxStaticText(
			row,
			wxID_ANY,
			FormatWhen(event),
			wxDefaultPosition,
			wxDefaultSize,
			wxST_ELLIPSIZE_END
		);
		timeLabel->SetForegroundColour(theme.color.secondaryLabel);
		wxBoxSizer* rowSizer = new wxBoxSizer(wxVERTICAL);
		rowSizer->Add(titleLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padX);
		rowSizer->Add(timeLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);
		rowSizer->AddSpacer(padY);
		row->SetSizer(rowSizer);
		this->listSizer->Add(row, 0, wxEXPAND);
	}
	this->scroller->FitInside();
	this->scroller->Layout();
	this->Layout();
}

void TodaysEventsPanel::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	this->heading->SetForegroundColour(theme.color.secondaryLabel);
	this->scroller->SetBackgroundColour(theme.color.window);
	for (wxWindow* child : this->scroller->GetChildren()) {
		child->SetBackgroundColour(theme.color.window);
		if (auto* text = dynamic_cast<wxStaticText*>(child)) {
			text->SetForegroundColour(theme.color.secondaryLabel);
			continue;
		}
		bool title = true;
		for (wxWindow* label : child->GetChildren()) {
			if (auto* text = dynamic_cast<wxStaticText*>(label)) {
				text->SetForegroundColour(title ? theme.color.label : theme.color.secondaryLabel);
				title = false;
			}
		}
	}
	this->Refresh();
}

void TodaysEventsPanel::OnSize(wxSizeEvent& event) {
	this->Refresh();
	event.Skip();
}

void TodaysEventsPanel::OnPaint(wxPaintEvent&) {
	wxPaintDC dc(this);
	wxGCDC gc(dc);
	Theme::Get().FillCanvas(gc, this, this->GetClientSize());
}
