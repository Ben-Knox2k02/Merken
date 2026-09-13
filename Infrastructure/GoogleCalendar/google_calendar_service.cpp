#include "google_calendar_service.h"

// dummy placeholder, does not call the real Google Calendar API yet
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const AppSettings& settings) {
	(void)settings;
	(void)this->httpClient;
	return event;
}

std::optional<CalendarEvent> GoogleCalendarService::GetEvent(const std::string& googleEventId, const AppSettings& settings) {
	(void)settings;
	(void)this->httpClient;
	if (googleEventId.empty()) {
		return std::nullopt;
	}
	CalendarEvent event(0, "", "", "");
	event.SetGoogleEventId(googleEventId);
	return event;
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const AppSettings& settings) {
	(void)event;
	(void)settings;
	(void)this->httpClient;
	return true;
}
