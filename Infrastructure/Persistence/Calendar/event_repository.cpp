#include "event_repository.h"
#include <wx/wxsqlite3.h>
#include <optional>

namespace {

const char* kEventColumns =
	"calendar_event_id, title, event_date, start_time, end_time, location, description, reminder_minutes, google_event_id";

CalendarEvent ReadEvent(wxSQLite3::ResultSet& result) {
	const std::string dateText = result.GetString(2).ToStdString(wxConvUTF8);
	const std::optional<Date> parsed = Date::Parse(dateText);
	const Date date = parsed.has_value() ? *parsed : Date::Create(1970, 1, 1).value();
	CalendarEvent event(
		result.GetInt(0),
		result.GetString(1).ToStdString(wxConvUTF8),
		date,
		result.GetString(3).ToStdString(wxConvUTF8),
		result.GetString(4).ToStdString(wxConvUTF8),
		result.GetString(5).ToStdString(wxConvUTF8),
		result.GetString(6).ToStdString(wxConvUTF8)
	);
	event.UpdateReminderMinutes(result.GetInt(7));
	event.SetGoogleEventId(result.GetString(8).ToStdString(wxConvUTF8));
	return event;
}

wxString Utf8(const std::string& value) {
	return wxString::FromUTF8(value.c_str());
}

void BindEvent(wxSQLite3::Statement& stmt, const CalendarEvent& event) {
	stmt.Bind(1, Utf8(event.GetTitle()));
	stmt.Bind(2, Utf8(event.GetDate().ToIso()));
	stmt.Bind(3, Utf8(event.GetStartTime()));
	stmt.Bind(4, Utf8(event.GetEndTime()));
	stmt.Bind(5, Utf8(event.GetLocation()));
	stmt.Bind(6, Utf8(event.GetDescription()));
	stmt.Bind(7, event.GetReminderMinutes());
	stmt.Bind(8, Utf8(event.GetGoogleEventId()));
}

}

std::vector<CalendarEvent> EventRepository::GetEvents() {
	std::vector<CalendarEvent> events;
	const std::string sql = std::string("SELECT ") + kEventColumns
		+ " FROM calendar_events ORDER BY event_date, start_time, calendar_event_id;";
	wxSQLite3::ResultSet result = this->db.GetConnection()->ExecuteQuery(sql);
	while (result.NextRow()) {
		events.push_back(ReadEvent(result));
	}
	return events;
}

std::vector<CalendarEvent> EventRepository::GetEventsForDate(const Date& date) {
	std::vector<CalendarEvent> events;
	const std::string sql = std::string("SELECT ") + kEventColumns
		+ " FROM calendar_events WHERE event_date = ? ORDER BY start_time, calendar_event_id;";
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(sql);
	stmt.Bind(1, Utf8(date.ToIso()));
	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		events.push_back(ReadEvent(result));
	}
	return events;
}

std::optional<CalendarEvent> EventRepository::GetEvent(int calendarEventId) {
	const std::string sql = std::string("SELECT ") + kEventColumns
		+ " FROM calendar_events WHERE calendar_event_id = ?;";
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(sql);
	stmt.Bind(1, calendarEventId);
	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) {
		return std::nullopt;
	}
	return ReadEvent(result);
}

int EventRepository::AddEvent(const CalendarEvent& event) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO calendar_events "
			"(title, event_date, start_time, end_time, location, description, reminder_minutes, google_event_id) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?);"
		);
		BindEvent(stmt, event);
		if (stmt.ExecuteUpdate() <= 0) {
			return 0;
		}
		return static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
	} catch (const wxSQLite3::Exception&) {
		return 0;
	}
}

bool EventRepository::UpdateEvent(const CalendarEvent& event) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"UPDATE calendar_events SET title = ?, event_date = ?, start_time = ?, end_time = ?, "
			"location = ?, description = ?, reminder_minutes = ?, google_event_id = ? "
			"WHERE calendar_event_id = ?;"
		);
		BindEvent(stmt, event);
		stmt.Bind(9, event.GetCalendarEventId());
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}

bool EventRepository::DeleteEvent(int calendarEventId) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"DELETE FROM calendar_events WHERE calendar_event_id = ?;"
		);
		stmt.Bind(1, calendarEventId);
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception&) {
		return false;
	}
}
