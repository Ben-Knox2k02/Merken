#include "dialog.h"
#include "centered_message.h"
#include "theme.h"
#include <cstdio>
#include <optional>

CardDialog::CardDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(0, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Front:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->frontCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
	this->frontCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->frontCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Back:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->backCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
	this->backCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->backCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALIGN_CENTER_VERTICAL);
	this->tagCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->tagCtrl, 1, wxEXPAND);

	wxButton* okButton = new wxButton(this, wxID_OK);
	wxButton* cancelButton = new wxButton(this, wxID_CANCEL);
	ApplyButtonPadding(okButton);
	ApplyButtonPadding(cancelButton);
	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(okButton);
	buttons->AddButton(cancelButton);
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	Theme::Get().StyleDialog(this);
	SizeAsThirdOfMainWindow(this);
}

DeckDialog::DeckDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->nameCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->descriptionCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->descriptionCtrl, 1, wxEXPAND);

	wxButton* okButton = new wxButton(this, wxID_OK);
	wxButton* cancelButton = new wxButton(this, wxID_CANCEL);
	ApplyButtonPadding(okButton);
	ApplyButtonPadding(cancelButton);
	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(okButton);
	buttons->AddButton(cancelButton);
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	Theme::Get().StyleDialog(this);
	SizeAsThirdOfMainWindow(this);
}

namespace {

const int kReminderMinutes[] = {5, 10, 15, 30, 60, 120, 1440};

void AppendHours(wxChoice* choice) {
	for (int hour = 0; hour < 24; ++hour) {
		choice->Append(wxString::Format("%02d", hour));
	}
	choice->SetSelection(0);
}

void AppendMinutes(wxChoice* choice) {
	for (int minute = 0; minute < 60; ++minute) {
		choice->Append(wxString::Format("%02d", minute));
	}
	choice->SetSelection(0);
}

void AppendReminders(wxChoice* choice) {
	const wxString labels[] = {
		"5 minutes",
		"10 minutes",
		"15 minutes",
		"30 minutes",
		"1 hour",
		"2 hours",
		"1 day"
	};
	for (const wxString& label : labels) {
		choice->Append(label);
	}
	choice->SetSelection(4);
}

void SelectClock(wxChoice* hours, wxChoice* minutes, const std::string& clock) {
	int hour = 0;
	int minute = 0;
	if (std::sscanf(clock.c_str(), "%d:%d", &hour, &minute) != 2
		|| hour < 0 || hour > 23
		|| minute < 0 || minute > 59) {
		hour = 0;
		minute = 0;
	}
	hours->SetSelection(hour);
	minutes->SetSelection(minute);
}

std::string ReadClock(const wxChoice* hours, const wxChoice* minutes) {
	int hour = hours->GetSelection();
	int minute = minutes->GetSelection();
	if (hour < 0) {
		hour = 0;
	}
	if (minute < 0) {
		minute = 0;
	}
	char buffer[6] = {0};
	std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
	return buffer;
}

wxString Utf8Text(const std::string& value) {
	const wxString text = wxString::FromUTF8(value.c_str());
	if (text.empty() && !value.empty()) {
		return wxString::From8BitData(value.c_str());
	}
	return text;
}

std::string ToUtf8(const wxString& value) {
	return value.ToStdString(wxConvUTF8);
}

}

EventDialog::EventDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(5, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Title:"), 0, wxALIGN_CENTER_VERTICAL);
	this->titleCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->titleCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Date:"), 0, wxALIGN_CENTER_VERTICAL);
	wxBoxSizer* dateRow = new wxBoxSizer(wxHORIZONTAL);
	this->monthCtrl = new wxChoice(this, wxID_ANY);
	for (int month = 0; month < 12; ++month) {
		this->monthCtrl->Append(wxDateTime::GetMonthName(static_cast<wxDateTime::Month>(month)));
	}
	this->monthCtrl->SetSelection(0);
	this->dayCtrl = new wxChoice(this, wxID_ANY);
	this->yearCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 9999, 2026);
	dateRow->Add(this->monthCtrl, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
	dateRow->Add(this->dayCtrl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
	dateRow->Add(this->yearCtrl, 0, wxALIGN_CENTER_VERTICAL);
	fields->Add(dateRow, 1, wxEXPAND);

	auto addClock = [&](wxChoice*& hours, wxChoice*& minutes) {
		wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
		hours = new wxChoice(this, wxID_ANY);
		minutes = new wxChoice(this, wxID_ANY);
		AppendHours(hours);
		AppendMinutes(minutes);
		row->Add(hours, 0, wxALIGN_CENTER_VERTICAL);
		row->Add(new wxStaticText(this, wxID_ANY, ":"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 4);
		row->Add(minutes, 0, wxALIGN_CENTER_VERTICAL);
		return row;
	};

	fields->Add(new wxStaticText(this, wxID_ANY, "Start:"), 0, wxALIGN_CENTER_VERTICAL);
	fields->Add(addClock(this->startHourCtrl, this->startMinuteCtrl), 0, wxALIGN_CENTER_VERTICAL);

	fields->Add(new wxStaticText(this, wxID_ANY, "End:"), 0, wxALIGN_CENTER_VERTICAL);
	fields->Add(addClock(this->endHourCtrl, this->endMinuteCtrl), 0, wxALIGN_CENTER_VERTICAL);

	fields->Add(new wxStaticText(this, wxID_ANY, "Location:"), 0, wxALIGN_CENTER_VERTICAL);
	this->locationCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->locationCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->descriptionCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->descriptionCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Reminder:"), 0, wxALIGN_CENTER_VERTICAL);
	this->reminderCtrl = new wxChoice(this, wxID_ANY);
	AppendReminders(this->reminderCtrl);
	fields->Add(this->reminderCtrl, 0, wxALIGN_CENTER_VERTICAL);

	wxButton* okButton = new wxButton(this, wxID_ANY, "OK");
	wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	ApplyButtonPadding(okButton);
	ApplyButtonPadding(cancelButton);
	okButton->SetDefault();
	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	buttons->AddStretchSpacer(1);
	buttons->Add(okButton, 0, wxRIGHT, 8);
	buttons->Add(cancelButton, 0);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);
	this->SetSizer(rootSizer);
	Theme::Get().StyleDialog(this);
	SizeAsThirdOfMainWindow(this);

	this->monthCtrl->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { this->RefreshDays(); });
	this->yearCtrl->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent&) { this->RefreshDays(); });
	this->yearCtrl->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { this->RefreshDays(); });
	okButton->Bind(wxEVT_BUTTON, &EventDialog::OnOk, this);

	const wxDateTime today = wxDateTime::Today();
	const std::optional<Date> todayDate = Date::Create(
		today.GetYear(),
		static_cast<int>(today.GetMonth()) + 1,
		today.GetDay()
	);
	if (todayDate.has_value()) {
		this->SetEventDate(*todayDate);
	} else {
		this->RefreshDays();
	}
	this->SetStartTime("09:00");
	this->SetEndTime("10:00");
	this->SetReminderMinutes(60);
}

void EventDialog::RefreshDays() {
	const int year = this->yearCtrl->GetValue();
	const int month = this->monthCtrl->GetSelection() + 1;
	int day = this->dayCtrl->GetSelection() + 1;
	const int days = (month >= 1 && month <= 12) ? Date::DaysInMonth(year, month) : 31;
	if (day < 1) {
		day = 1;
	}
	if (day > days) {
		day = days;
	}
	this->dayCtrl->Clear();
	for (int value = 1; value <= days; ++value) {
		this->dayCtrl->Append(wxString::Format("%d", value));
	}
	this->dayCtrl->SetSelection(day - 1);
}

void EventDialog::OnOk(wxCommandEvent&) {
	if (this->titleCtrl->GetValue().Trim().Trim(false).IsEmpty()) {
		ShowCenteredMessage(this, "Enter a title.", "Event", wxOK | wxICON_WARNING);
		return;
	}
	this->EndModal(wxID_OK);
}

void EventDialog::SetEventTitle(const std::string& title) {
	this->titleCtrl->SetValue(Utf8Text(title));
}

std::string EventDialog::GetEventTitle() const {
	return ToUtf8(this->titleCtrl->GetValue().Trim().Trim(false));
}

void EventDialog::SetEventDate(const Date& date) {
	const int monthIndex = date.GetMonth() - 1;
	if (monthIndex >= 0 && monthIndex < static_cast<int>(this->monthCtrl->GetCount())) {
		this->monthCtrl->SetSelection(monthIndex);
	}
	this->yearCtrl->SetValue(date.GetYear());
	this->RefreshDays();
	const int dayIndex = date.GetDay() - 1;
	if (dayIndex >= 0 && dayIndex < static_cast<int>(this->dayCtrl->GetCount())) {
		this->dayCtrl->SetSelection(dayIndex);
	}
}

Date EventDialog::GetEventDate() const {
	const int month = this->monthCtrl->GetSelection() + 1;
	const int day = this->dayCtrl->GetSelection() + 1;
	const int year = this->yearCtrl->GetValue();
	const std::optional<Date> created = Date::Create(year, month, day);
	if (!created.has_value()) {
		return Date::Create(1970, 1, 1).value();
	}
	return *created;
}

void EventDialog::SetStartTime(const std::string& clock) {
	SelectClock(this->startHourCtrl, this->startMinuteCtrl, clock);
}

std::string EventDialog::GetStartTime() const {
	return ReadClock(this->startHourCtrl, this->startMinuteCtrl);
}

void EventDialog::SetEndTime(const std::string& clock) {
	SelectClock(this->endHourCtrl, this->endMinuteCtrl, clock);
}

std::string EventDialog::GetEndTime() const {
	return ReadClock(this->endHourCtrl, this->endMinuteCtrl);
}

void EventDialog::SetLocation(const std::string& location) {
	this->locationCtrl->SetValue(Utf8Text(location));
}

std::string EventDialog::GetLocation() const {
	return ToUtf8(this->locationCtrl->GetValue());
}

void EventDialog::SetDescription(const std::string& description) {
	this->descriptionCtrl->SetValue(Utf8Text(description));
}

std::string EventDialog::GetDescription() const {
	return ToUtf8(this->descriptionCtrl->GetValue());
}

void EventDialog::SetReminderMinutes(int minutes) {
	for (int index = 0; index < static_cast<int>(sizeof(kReminderMinutes) / sizeof(kReminderMinutes[0])); ++index) {
		if (kReminderMinutes[index] == minutes) {
			this->reminderCtrl->SetSelection(index);
			return;
		}
	}
	this->reminderCtrl->SetSelection(4);
}

int EventDialog::GetReminderMinutes() const {
	const int index = this->reminderCtrl->GetSelection();
	const int count = static_cast<int>(sizeof(kReminderMinutes) / sizeof(kReminderMinutes[0]));
	if (index < 0 || index >= count) {
		return 60;
	}
	return kReminderMinutes[index];
}
