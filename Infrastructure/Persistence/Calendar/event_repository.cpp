#include "event_repository.h"
#include <wx/wxsqlite3.h>
#include <wx/log.h>

std::vector<CalendarEvent> EventRepository::GetEventsForDate(const Date& date) {
	std::vector<CalendarEvent> events;
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT calendar_event_id, title, event_date, start_time, end_time, location, description, reminder_minutes "
		"FROM calendar_events WHERE event_date = ? ORDER BY start_time;"
	);
	stmt.Bind(1, wxString(date.ToIso()));

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	while (result.NextRow()) {
		std::string dateStr = result.GetString(2).ToStdString();
		int year = 0, month = 0, day = 0;
		sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day);
		auto dateOpt = Date::Create(year, month, day);
		Date eventDate = dateOpt.has_value() ? *dateOpt : Date::Create(2026, 1, 1).value();

		// Construct with the 7 supported parameters
		CalendarEvent ev(
			result.GetInt(0),
			result.GetString(1).ToStdString(),
			eventDate,
			result.GetString(3).ToStdString(),
			result.GetString(4).ToStdString(),
			result.GetString(5).ToStdString(),
			result.GetString(6).ToStdString()
		);
		// Set reminder minutes via its setter method
		ev.UpdateReminderMinutes(result.GetInt(7));

		events.push_back(std::move(ev));
	}

	return events;
}

std::optional<CalendarEvent> EventRepository::GetEvent(int eventId) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"SELECT calendar_event_id, title, event_date, start_time, end_time, location, description, reminder_minutes "
		"FROM calendar_events WHERE calendar_event_id = ?;"
	);
	stmt.Bind(1, eventId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if (!result.NextRow()) { return std::nullopt; }

	std::string dateStr = result.GetString(2).ToStdString();
	int year = 0, month = 0, day = 0;
	sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day);
	auto dateOpt = Date::Create(year, month, day);
	Date eventDate = dateOpt.has_value() ? *dateOpt : Date::Create(2026, 1, 1).value();

	CalendarEvent ev(
		result.GetInt(0),
		result.GetString(1).ToStdString(),
		eventDate,
		result.GetString(3).ToStdString(),
		result.GetString(4).ToStdString(),
		result.GetString(5).ToStdString(),
		result.GetString(6).ToStdString()
	);
	ev.UpdateReminderMinutes(result.GetInt(7));

	return ev;
}

std::vector<CalendarEvent> EventRepository::GetEvents() {
    std::vector<CalendarEvent> events;
    wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
        "SELECT calendar_event_id, title, event_date, start_time, end_time, location, description, reminder_minutes "
        "FROM calendar_events ORDER BY event_date, start_time;"
    );
    wxSQLite3::ResultSet result = stmt.ExecuteQuery();
    while (result.NextRow()) {
        std::string dateStr = result.GetString(2).ToStdString();
        int year = 0, month = 0, day = 0;
        sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day);
        auto dateOpt = Date::Create(year, month, day);
        Date eventDate = dateOpt.has_value() ? *dateOpt : Date::Create(2026, 1, 1).value();

        CalendarEvent ev(
            result.GetInt(0),
            result.GetString(1).ToStdString(),
            eventDate,
            result.GetString(3).ToStdString(),
            result.GetString(4).ToStdString(),
            result.GetString(5).ToStdString(),
            result.GetString(6).ToStdString()
        );
        ev.UpdateReminderMinutes(result.GetInt(7));
        events.push_back(std::move(ev));
    }
    return events;
}

int EventRepository::AddEvent(const CalendarEvent& event) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"INSERT INTO calendar_events (title, event_date, start_time, end_time, location, description, reminder_minutes) "
			"VALUES (?, ?, ?, ?, ?, ?, ?);"
		);
		stmt.Bind(1, wxString(event.GetTitle()));
		stmt.Bind(2, wxString(event.GetDate().ToIso()));
		stmt.Bind(3, wxString(event.GetStartTime()));
		stmt.Bind(4, wxString(event.GetEndTime()));
		stmt.Bind(5, wxString(event.GetLocation()));
		stmt.Bind(6, wxString(event.GetDescription()));
		stmt.Bind(7, event.GetReminderMinutes());

		if (stmt.ExecuteUpdate() <= 0) { 
			wxLogDebug("AddEvent Warning: ExecuteUpdate returned <= 0 for event: %s", event.GetTitle().c_str());
			return 0; 
		}
		
		int newId = static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
		wxLogDebug("AddEvent Success: Inserted event '%s' with database ID %d", event.GetTitle().c_str(), newId);
		return newId;
	} 
	catch (const wxSQLite3::Exception& e) {
		wxLogDebug("AddEvent SQLite Exception: %s", e.GetMessage());
		return 0;
	}
}

bool EventRepository::UpdateEvent(const CalendarEvent& event) {
	wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
		"UPDATE calendar_events SET title = ?, event_date = ?, start_time = ?, end_time = ?, "
		"location = ?, description = ?, reminder_minutes = ? WHERE calendar_event_id = ?;"
	);
	stmt.Bind(1, wxString(event.GetTitle()));
	stmt.Bind(2, wxString(event.GetDate().ToIso()));
	stmt.Bind(3, wxString(event.GetStartTime()));
	stmt.Bind(4, wxString(event.GetEndTime()));
	stmt.Bind(5, wxString(event.GetLocation()));
	stmt.Bind(6, wxString(event.GetDescription()));
	stmt.Bind(7, event.GetReminderMinutes());
	stmt.Bind(8, event.GetCalendarEventId());

	return stmt.ExecuteUpdate() > 0;
}

bool EventRepository::DeleteEvent(int eventId) {
	try {
		wxSQLite3::Statement stmt = this->db.GetConnection()->PrepareStatement(
			"DELETE FROM calendar_events WHERE calendar_event_id = ?;"
		);
		stmt.Bind(1, eventId);
		return stmt.ExecuteUpdate() > 0;
	} catch (const wxSQLite3::Exception& e) {
		wxLogDebug("SQLite Delete Error: %s", e.GetMessage());
		return false;
	}
}
