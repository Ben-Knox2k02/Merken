#include "calendar_panel.h"
#include "chevron_button.h"
#include "mainframe.h"
#include "theme.h"
#include "app.h"
#include "dialog.h"
#include "centered_message.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include "../../Application/UseCases/Calendar/CreateEvent/create_event_usecase.h"
#include "../../Application/UseCases/Calendar/UpdateEvent/update_event_usecase.h"
#include "../../Application/UseCases/Calendar/DeleteEvent/delete_event_usecase.h"
#include <wx/dcbuffer.h>
#include <wx/statline.h>
#include <optional>

wxDECLARE_APP(App);

namespace {

wxString Utf8Text(const std::string& value) {
	const wxString text = wxString::FromUTF8(value.c_str());
	if (text.empty() && !value.empty()) {
		return wxString::From8BitData(value.c_str());
	}
	return text;
}

wxString ReminderLabel(int minutes) {
	switch (minutes) {
		case 5: return "5 minutes";
		case 10: return "10 minutes";
		case 15: return "15 minutes";
		case 30: return "30 minutes";
		case 60: return "1 hour";
		case 120: return "2 hours";
		case 1440: return "1 day";
		default: return wxString();
	}
}

}

MonthCalendar::MonthCalendar(wxWindow* parent)
	: wxPanel(parent),
	  visibleYear(1970),
	  visibleMonth(1),
	  selected(Date::Create(1970, 1, 1).value()) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	const wxDateTime today = wxDateTime::Today();
	const std::optional<Date> created = Date::Create(
		today.GetYear(),
		static_cast<int>(today.GetMonth()) + 1,
		today.GetDay()
	);
	if (created.has_value()) {
		this->SetDate(*created);
	}
	this->SetMinSize(wxSize(this->CellSize() * 7, this->HeaderHeight() + this->CellSize() * 6));
	this->Bind(wxEVT_PAINT, &MonthCalendar::OnPaint, this);
	this->Bind(wxEVT_LEFT_UP, &MonthCalendar::OnClick, this);
}

void MonthCalendar::SetDate(const Date& date) {
	this->selected = date;
	this->visibleYear = date.GetYear();
	this->visibleMonth = date.GetMonth();
	this->Refresh();
}

const Date& MonthCalendar::GetDate() const {
	return this->selected;
}

void MonthCalendar::ShiftMonth(int delta) {
	int year = this->visibleYear;
	int month = this->visibleMonth + delta;
	while (month > 12) {
		month -= 12;
		year += 1;
	}
	while (month < 1) {
		month += 12;
		year -= 1;
	}
	if (year < 1) {
		year = 1;
		month = 1;
	}
	if (year > 9999) {
		year = 9999;
		month = 12;
	}

	int day = this->selected.GetDay();
	const int days = Date::DaysInMonth(year, month);
	if (day > days) {
		day = days;
	}
	const std::optional<Date> created = Date::Create(year, month, day);
	if (!created.has_value()) {
		return;
	}
	this->visibleYear = year;
	this->visibleMonth = month;
	this->selected = *created;
	this->Refresh();
	this->Notify();
}

int MonthCalendar::CellSize() const {
	const int width = this->GetClientSize().GetWidth();
	if (width < 7) {
		return this->FromDIP(36);
	}
	const int fitted = width / 7;
	return fitted < 1 ? 1 : fitted;
}

int MonthCalendar::ColumnWidth(int column) const {
	const int width = this->GetClientSize().GetWidth();
	if (width < 7) {
		return this->CellSize();
	}
	return this->CellSize() + (column < width % 7 ? 1 : 0);
}

int MonthCalendar::ColumnLeft(int column) const {
	int left = 0;
	for (int index = 0; index < column; ++index) {
		left += this->ColumnWidth(index);
	}
	return left;
}

int MonthCalendar::HeaderHeight() const {
	return this->FromDIP(22);
}

int MonthCalendar::LeadDays() const {
	wxDateTime first;
	first.Set(1, static_cast<wxDateTime::Month>(this->visibleMonth - 1), this->visibleYear);
	if (!first.IsValid()) {
		return 0;
	}
	return static_cast<int>(first.GetWeekDay());
}

wxRect MonthCalendar::DayRect(int row, int column) const {
	const int cell = this->CellSize();
	const int columnWidth = this->ColumnWidth(column);
	int gap = this->FromDIP(2);
	if (gap * 2 >= cell || gap * 2 >= columnWidth) {
		gap = 0;
	}
	return wxRect(
		this->ColumnLeft(column) + gap,
		this->HeaderHeight() + row * cell + gap,
		columnWidth - gap * 2,
		cell - gap * 2
	);
}

int MonthCalendar::HitDay(const wxPoint& position) const {
	if (position.y < this->HeaderHeight()) {
		return 0;
	}
	const int cell = this->CellSize();
	if (cell < 1) {
		return 0;
	}
	int column = -1;
	int x = 0;
	for (int index = 0; index < 7; ++index) {
		const int span = this->ColumnWidth(index);
		if (position.x >= x && position.x < x + span) {
			column = index;
			break;
		}
		x += span;
	}
	const int row = (position.y - this->HeaderHeight()) / cell;
	if (column < 0 || column > 6 || row < 0 || row > 5) {
		return 0;
	}
	const int day = row * 7 + column - this->LeadDays() + 1;
	const int days = Date::DaysInMonth(this->visibleYear, this->visibleMonth);
	if (day < 1 || day > days) {
		return 0;
	}
	return day;
}

void MonthCalendar::Notify() {
	wxCommandEvent changed(wxEVT_BUTTON, this->GetId());
	changed.SetEventObject(this);
	this->ProcessWindowEvent(changed);
}

void MonthCalendar::OnPaint(wxPaintEvent&) {
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.Clear();

	const wxColour windowText = this->GetForegroundColour();
	const wxColour highlight = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
	const wxColour highlightText = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
	const wxFont dayFont = this->GetFont();
	const wxDateTime today = wxDateTime::Today();
	const int lead = this->LeadDays();
	const int days = Date::DaysInMonth(this->visibleYear, this->visibleMonth);

	dc.SetFont(dayFont.Smaller());
	dc.SetTextForeground(windowText);
	for (int column = 0; column < 7; ++column) {
		const wxString name = wxDateTime::GetWeekDayName(
			static_cast<wxDateTime::WeekDay>(column),
			wxDateTime::Name_Abbr
		);
		const wxRect header(this->ColumnLeft(column), 0, this->ColumnWidth(column), this->HeaderHeight());
		const wxSize extent = dc.GetTextExtent(name);
		dc.DrawText(
			name,
			header.x + (header.width - extent.GetWidth()) / 2,
			header.y + (header.height - extent.GetHeight()) / 2
		);
	}

	dc.SetFont(dayFont);
	for (int day = 1; day <= days; ++day) {
		const int slot = lead + day - 1;
		const int row = slot / 7;
		const int column = slot % 7;
		const wxRect rect = this->DayRect(row, column);
		const bool selectedDay = this->selected.GetYear() == this->visibleYear
			&& this->selected.GetMonth() == this->visibleMonth
			&& this->selected.GetDay() == day;
		const bool todayDay = today.IsValid()
			&& today.GetYear() == this->visibleYear
			&& static_cast<int>(today.GetMonth()) + 1 == this->visibleMonth
			&& static_cast<int>(today.GetDay()) == day;

		if (selectedDay) {
			dc.SetBrush(wxBrush(highlight));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetTextForeground(highlightText);
			dc.DrawRectangle(rect);
		} else if (todayDay) {
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(wxPen(highlight));
			dc.SetTextForeground(windowText);
			dc.DrawRectangle(rect);
		} else {
			dc.SetTextForeground(windowText);
		}

		const wxString label = wxString::Format("%d", day);
		const wxSize extent = dc.GetTextExtent(label);
		dc.DrawText(
			label,
			rect.x + (rect.width - extent.GetWidth()) / 2,
			rect.y + (rect.height - extent.GetHeight()) / 2
		);
	}
}

void MonthCalendar::OnClick(wxMouseEvent& event) {
	const int day = this->HitDay(event.GetPosition());
	if (day < 1) {
		return;
	}
	const std::optional<Date> created = Date::Create(this->visibleYear, this->visibleMonth, day);
	if (!created.has_value() || *created == this->selected) {
		return;
	}
	this->selected = *created;
	this->Refresh();
	this->Notify();
}

CalendarPanel::CalendarPanel(wxWindow* parent) : wxPanel(parent) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetDoubleBuffered(true);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	const wxColour background = this->GetBackgroundColour();
	this->calendarColumn = new wxPanel(this, wxID_ANY);
	this->eventColumn = new wxPanel(this, wxID_ANY);
	this->calendarColumn->SetBackgroundColour(background);
	this->eventColumn->SetBackgroundColour(background);

	wxBoxSizer* monthRow = new wxBoxSizer(wxHORIZONTAL);
	const wxSize previousSize = ChevronButtonSize(this, wxString(L"\u2190"));
	const wxSize nextSize = ChevronButtonSize(this, wxString(L"\u2192"));
	const wxSize chevronSize(
		previousSize.GetWidth() > nextSize.GetWidth() ? previousSize.GetWidth() : nextSize.GetWidth(),
		previousSize.GetHeight() > nextSize.GetHeight() ? previousSize.GetHeight() : nextSize.GetHeight()
	);
	this->previousButton = new ChevronButton(this->calendarColumn, wxString(L"\u2190"), chevronSize);
	this->nextButton = new ChevronButton(this->calendarColumn, wxString(L"\u2192"), chevronSize);
	this->previousButton->SetMinSize(chevronSize);
	this->nextButton->SetMinSize(chevronSize);
	this->previousButton->SetMaxSize(chevronSize);
	this->nextButton->SetMaxSize(chevronSize);
	this->monthLabel = new wxStaticText(
		this->calendarColumn,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_CENTER_HORIZONTAL | wxST_ELLIPSIZE_END
	);
	this->monthLabel->SetMinSize(wxSize(this->FromDIP(48), -1));
	monthRow->Add(this->previousButton, 0, wxALIGN_CENTER_VERTICAL);
	monthRow->Add(this->monthLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 8);
	monthRow->Add(this->nextButton, 0, wxALIGN_CENTER_VERTICAL);

	this->monthGrid = new MonthCalendar(this->calendarColumn);
	const Theme& theme = Theme::Get();
	const int columnPad = this->FromDIP(theme.size.cardPad);
	this->header = new wxStaticText(this->calendarColumn, wxID_ANY, "Calendar");
	wxFont calendarTitleFont = this->header->GetFont();
	calendarTitleFont.SetPointSize(calendarTitleFont.GetPointSize() + 3);
	this->header->SetFont(calendarTitleFont);
	this->header->SetForegroundColour(theme.color.label);
	this->backButton = new ChevronButton(this->calendarColumn, wxString(L"\u2039"), chevronSize);
	this->backButton->SetMinSize(chevronSize);
	this->backButton->SetMaxSize(chevronSize);
	wxBoxSizer* titleRow = new wxBoxSizer(wxHORIZONTAL);
	titleRow->Add(this->backButton, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, this->FromDIP(theme.space.sm));
	titleRow->Add(this->header, 1, wxALIGN_CENTER_VERTICAL);
	wxBoxSizer* monthBlock = new wxBoxSizer(wxVERTICAL);
	monthBlock->Add(monthRow, 0, wxEXPAND);
	monthBlock->Add(this->monthGrid, 0, wxEXPAND | wxTOP, 8);
	wxBoxSizer* calendarSizer = new wxBoxSizer(wxVERTICAL);
	calendarSizer->AddSpacer(this->FromDIP(theme.size.panelPad));
	calendarSizer->Add(titleRow, 0, wxEXPAND | wxLEFT | wxRIGHT, columnPad);
	calendarSizer->Add(monthBlock, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, columnPad);
	this->calendarColumn->SetSizer(calendarSizer);

	const int pad = this->FromDIP(theme.size.panelPad);
	wxStaticText* eventsTitle = new wxStaticText(this->eventColumn, wxID_ANY, "Events");
	wxFont titleFont = eventsTitle->GetFont();
	titleFont.SetPointSize(titleFont.GetPointSize() + 3);
	eventsTitle->SetFont(titleFont);
	eventsTitle->SetForegroundColour(theme.color.label);

	this->addButton = new wxButton(this->eventColumn, wxID_ANY, "Add Event");
	this->editButton = new wxButton(this->eventColumn, wxID_ANY, "Edit Event");
	this->deleteButton = new wxButton(this->eventColumn, wxID_ANY, "Delete Event");
	ApplyButtonPadding(this->addButton);
	ApplyButtonPadding(this->editButton);
	ApplyButtonPadding(this->deleteButton);
	int sharedWidth = this->addButton->GetMinSize().GetWidth();
	if (this->editButton->GetMinSize().GetWidth() > sharedWidth) {
		sharedWidth = this->editButton->GetMinSize().GetWidth();
	}
	if (this->deleteButton->GetMinSize().GetWidth() > sharedWidth) {
		sharedWidth = this->deleteButton->GetMinSize().GetWidth();
	}
	const wxSize sharedSize(sharedWidth, wxButton::GetDefaultSize(this).GetHeight());
	this->addButton->SetMinSize(sharedSize);
	this->editButton->SetMinSize(sharedSize);
	this->deleteButton->SetMinSize(sharedSize);
	this->editButton->Enable(false);
	this->deleteButton->Enable(false);

	wxBoxSizer* headerButtons = new wxBoxSizer(wxHORIZONTAL);
	const int buttonGap = this->FromDIP(theme.space.xs);
	headerButtons->Add(this->addButton, 0, wxRIGHT | wxALIGN_BOTTOM, buttonGap);
	headerButtons->Add(this->editButton, 0, wxRIGHT | wxALIGN_BOTTOM, buttonGap);
	headerButtons->Add(this->deleteButton, 0, wxALIGN_BOTTOM);

	wxBoxSizer* headerRow = new wxBoxSizer(wxHORIZONTAL);
	headerRow->Add(eventsTitle, 1, wxEXPAND | wxRIGHT, this->FromDIP(theme.space.md));
	headerRow->Add(headerButtons, 0, wxALIGN_TOP);

	this->statusLabel = new wxStaticText(this->eventColumn, wxID_ANY, wxEmptyString);
	this->statusLabel->SetForegroundColour(theme.color.secondaryLabel);
	this->statusLabel->Hide();

	this->eventList = new wxListCtrl(
		this->eventColumn,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxLC_REPORT | wxLC_SINGLE_SEL
	);
	this->eventList->AppendColumn("Title", wxLIST_FORMAT_LEFT, this->FromDIP(180));
	this->eventList->AppendColumn("Start", wxLIST_FORMAT_LEFT, this->FromDIP(80));
	this->eventList->AppendColumn("End", wxLIST_FORMAT_LEFT, this->FromDIP(80));
	this->eventList->AppendColumn("Location", wxLIST_FORMAT_LEFT, this->FromDIP(160));
	this->eventList->AppendColumn("Description", wxLIST_FORMAT_LEFT, this->FromDIP(200));
	this->eventList->AppendColumn("Reminder", wxLIST_FORMAT_LEFT, this->FromDIP(120));
	this->eventList->SetMinSize(wxSize(-1, this->FromDIP(120)));

	wxBoxSizer* eventSizer = new wxBoxSizer(wxVERTICAL);
	eventSizer->AddSpacer(pad);
	eventSizer->Add(headerRow, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
	eventSizer->AddSpacer(this->FromDIP(theme.space.sm));
	eventSizer->Add(new wxStaticLine(this->eventColumn, wxID_ANY), 0, wxEXPAND);
	eventSizer->Add(this->statusLabel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
	eventSizer->Add(this->eventList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
	this->eventColumn->SetSizer(eventSizer);

	wxBoxSizer* columns = new wxBoxSizer(wxHORIZONTAL);
	this->columnSplit = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	columns->Add(this->calendarColumn, 25, wxEXPAND);
	columns->Add(this->columnSplit, 0, wxEXPAND);
	columns->Add(this->eventColumn, 75, wxEXPAND);
	this->rootSizer->Add(columns, 1, wxEXPAND);

	this->SetSizer(this->rootSizer);
	this->Bind(wxEVT_PAINT, &CalendarPanel::OnPaint, this);
	this->Bind(wxEVT_SIZE, &CalendarPanel::OnSize, this);
	this->previousButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnPrevious, this);
	this->nextButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnNext, this);
	this->monthGrid->Bind(wxEVT_BUTTON, &CalendarPanel::OnDateChanged, this);
	this->addButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnAdd, this);
	this->editButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnEdit, this);
	this->deleteButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnDelete, this);
	this->backButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnBack, this);
	this->eventList->Bind(wxEVT_LIST_ITEM_SELECTED, &CalendarPanel::OnSelectionChanged, this);
	this->eventList->Bind(wxEVT_LIST_ITEM_DESELECTED, &CalendarPanel::OnSelectionChanged, this);

	this->SyncMonthLabel();
	this->LoadEvents();
}

void CalendarPanel::SyncMonthLabel() {
	const Date& date = this->monthGrid->GetDate();
	wxDateTime when;
	when.Set(1, static_cast<wxDateTime::Month>(date.GetMonth() - 1), date.GetYear());
	if (when.IsValid()) {
		this->monthLabel->SetLabel(when.Format("%B %Y"));
	}
}

void CalendarPanel::LoadEvents() {
	auto useCase = wxGetApp().GetInjector().create<GetEventsUseCase>();
	this->ShowDay(useCase.ExecuteForDate(this->monthGrid->GetDate()));
}

void CalendarPanel::ShowDay(const GetEventsResponse& response) {
	this->dayEvents = response.events;
	this->eventList->DeleteAllItems();
	for (const EventResponse& event : this->dayEvents) {
		const long index = this->eventList->InsertItem(
			this->eventList->GetItemCount(),
			Utf8Text(event.title)
		);
		this->eventList->SetItem(index, 1, Utf8Text(event.startTime));
		this->eventList->SetItem(index, 2, Utf8Text(event.endTime));
		this->eventList->SetItem(index, 3, Utf8Text(event.location));
		this->eventList->SetItem(index, 4, Utf8Text(event.description));
		this->eventList->SetItem(index, 5, ReminderLabel(event.reminderMinutes));
		this->eventList->SetItemData(index, event.calendarEventId);
	}
	if (this->dayEvents.empty()) {
		const long index = this->eventList->InsertItem(0, "No events.");
		this->eventList->SetItemTextColour(index, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
		this->eventList->SetItemData(index, 0);
	}

	wxString status;
	if (this->dayEvents.size() == 1) {
		status = "1 event";
	} else if (this->dayEvents.size() > 1) {
		status = wxString::Format("%d events", static_cast<int>(this->dayEvents.size()));
	}
	if (!this->notice.empty()) {
		status << " " << this->notice;
		this->notice.clear();
	}
	this->statusLabel->SetLabel(status);
	this->statusLabel->Show(!status.empty());
	this->UpdateActionButtons();
	this->Layout();
}

void CalendarPanel::UpdateActionButtons() {
	const bool hasSelection = this->SelectedEventId() != 0;
	this->editButton->Enable(hasSelection);
	this->deleteButton->Enable(hasSelection);
}

int CalendarPanel::SelectedEventId() const {
	const long index = this->eventList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index < 0) {
		return 0;
	}
	return static_cast<int>(this->eventList->GetItemData(index));
}

const EventResponse* CalendarPanel::SelectedEvent() const {
	const int eventId = this->SelectedEventId();
	if (eventId == 0) {
		return nullptr;
	}
	for (const EventResponse& event : this->dayEvents) {
		if (event.calendarEventId == eventId) {
			return &event;
		}
	}
	return nullptr;
}

void CalendarPanel::ApplyColumnWidths() {
	if (this->calendarColumn == nullptr || this->eventColumn == nullptr) {
		return;
	}
	const int pad = this->FromDIP(10);
	int gap = 0;
	if (this->columnSplit != nullptr) {
		gap = this->columnSplit->GetBestSize().GetWidth();
	}
	if (gap < 1) {
		gap = 1;
	}
	const int inner = this->GetClientSize().GetWidth() - pad - gap - pad;
	if (inner <= 0) {
		return;
	}
	const int calendarWidth = inner * 25 / 100;
	const int eventWidth = inner - calendarWidth;
	if (calendarWidth <= 0 || eventWidth <= 0) {
		return;
	}
	if (this->calendarColumn->GetMinSize().GetWidth() == calendarWidth
		&& this->eventColumn->GetMinSize().GetWidth() == eventWidth) {
		return;
	}

	this->calendarColumn->SetMinSize(wxSize(calendarWidth, -1));
	this->calendarColumn->SetMaxSize(wxSize(calendarWidth, -1));
	this->eventColumn->SetMinSize(wxSize(eventWidth, -1));
	this->eventColumn->SetMaxSize(wxSize(eventWidth, -1));

	const int inset = this->FromDIP(Theme::Get().size.cardPad);
	int gridWidth = calendarWidth - inset * 2;
	if (gridWidth < 7) {
		gridWidth = 7;
	}
	int cell = gridWidth / 7;
	if (cell < 1) {
		cell = 1;
	}
	this->monthGrid->SetMinSize(wxSize(gridWidth, this->FromDIP(22) + cell * 6));
	this->monthGrid->SetMaxSize(wxSize(gridWidth, this->FromDIP(22) + cell * 6));
	const int listInset = this->FromDIP(Theme::Get().size.panelPad);
	int listWidth = eventWidth - listInset * 2;
	if (listWidth < 1) {
		listWidth = eventWidth;
	}
	this->eventList->SetMinSize(wxSize(listWidth, this->FromDIP(120)));
	this->Layout();
	this->monthGrid->Refresh();
}

void CalendarPanel::OnSize(wxSizeEvent& event) {
	this->ApplyColumnWidths();
	event.Skip();
}

void CalendarPanel::OnPaint(wxPaintEvent&) {
	const wxColour background = this->GetBackgroundColour();
	if (this->calendarColumn != nullptr && this->calendarColumn->GetBackgroundColour() != background) {
		this->calendarColumn->SetBackgroundColour(background);
		this->eventColumn->SetBackgroundColour(background);
	}
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(background));
	dc.Clear();
}

void CalendarPanel::OnBack(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}

void CalendarPanel::OnPrevious(wxCommandEvent&) {
	this->monthGrid->ShiftMonth(-1);
}

void CalendarPanel::OnNext(wxCommandEvent&) {
	this->monthGrid->ShiftMonth(1);
}

void CalendarPanel::OnDateChanged(wxCommandEvent&) {
	this->SyncMonthLabel();
	this->LoadEvents();
}

void CalendarPanel::OnAdd(wxCommandEvent&) {
	EventDialog dialog(this, "Add Event");
	dialog.SetEventDate(this->monthGrid->GetDate());
	dialog.SetStartTime("09:00");
	dialog.SetEndTime("10:00");
	dialog.SetReminderMinutes(60);
	if (dialog.ShowModal() != wxID_OK) {
		return;
	}

	CreateEventRequest request;
	request.title = dialog.GetEventTitle();
	request.date = dialog.GetEventDate();
	request.description = dialog.GetDescription();
	request.startTime = dialog.GetStartTime();
	request.endTime = dialog.GetEndTime();
	request.location = dialog.GetLocation();
	request.reminderMinutes = dialog.GetReminderMinutes();

	this->addButton->Enable(false);
	auto useCase = wxGetApp().GetInjector().create<CreateEventUseCase>();
	const CreateEventResponse response = useCase.Execute(request);
	this->addButton->Enable(true);
	if (response.calendarEventId <= 0) {
		ShowCenteredMessage(this, "Could not save the event.", "Calendar", wxOK | wxICON_ERROR);
		return;
	}

	this->monthGrid->SetDate(response.date);
	this->notice = response.googleEventId.empty()
		? wxString("Saved on this computer.")
		: wxString("Saved to Google Calendar.");
	this->SyncMonthLabel();
	this->LoadEvents();
}

void CalendarPanel::OnEdit(wxCommandEvent&) {
	const EventResponse* selected = this->SelectedEvent();
	if (selected == nullptr) {
		return;
	}

	EventDialog dialog(this, "Edit Event");
	dialog.SetEventTitle(selected->title);
	dialog.SetEventDate(selected->date);
	dialog.SetStartTime(selected->startTime);
	dialog.SetEndTime(selected->endTime);
	dialog.SetLocation(selected->location);
	dialog.SetDescription(selected->description);
	dialog.SetReminderMinutes(selected->reminderMinutes);
	if (dialog.ShowModal() != wxID_OK) {
		return;
	}

	UpdateEventRequest request;
	request.calendarEventId = selected->calendarEventId;
	request.title = dialog.GetEventTitle();
	request.date = dialog.GetEventDate();
	request.description = dialog.GetDescription();
	request.startTime = dialog.GetStartTime();
	request.endTime = dialog.GetEndTime();
	request.location = dialog.GetLocation();
	request.reminderMinutes = dialog.GetReminderMinutes();

	this->editButton->Enable(false);
	auto useCase = wxGetApp().GetInjector().create<UpdateEventUseCase>();
	const bool updated = useCase.Execute(request);
	this->editButton->Enable(true);
	if (!updated) {
		ShowCenteredMessage(this, "Could not update the event.", "Calendar", wxOK | wxICON_ERROR);
		return;
	}

	this->monthGrid->SetDate(request.date);
	this->notice = "Updated.";
	this->SyncMonthLabel();
	this->LoadEvents();
}

void CalendarPanel::OnDelete(wxCommandEvent&) {
	const EventResponse* selected = this->SelectedEvent();
	if (selected == nullptr) {
		return;
	}
	const int eventId = selected->calendarEventId;
	const bool onGoogle = !selected->googleEventId.empty();
	const wxString message = onGoogle
		? "Delete this event from Merken? It stays on Google Calendar."
		: "Delete this event?";
	if (ShowCenteredMessage(this, message, "Delete Event", wxYES_NO | wxICON_WARNING) != wxID_YES) {
		return;
	}

	DeleteEventRequest request;
	request.calendarEventId = eventId;
	auto useCase = wxGetApp().GetInjector().create<DeleteEventUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not delete the event.", "Calendar", wxOK | wxICON_ERROR);
		return;
	}
	this->LoadEvents();
}

void CalendarPanel::OnSelectionChanged(wxListEvent&) {
	this->UpdateActionButtons();
}
