#include "google_calendar_service.h"

// dummy placeholder, does not call the real Google Calendar API yet
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const std::string& apiKey) {
	(void)apiKey;
	(void)this->httpClient;
	return event;
}

std::optional<CalendarEvent> GoogleCalendarService::GetEvent(const std::string& googleEventId, const std::string& apiKey) {
	(void)apiKey;
	(void)this->httpClient;
	if (googleEventId.empty()) {
		return std::nullopt;
	}
	CalendarEvent event(0, "", "", "");
	event.SetGoogleEventId(googleEventId);
	return event;
}

std::vector<CalendarEvent> GoogleCalendarService::GetEvents(const std::string& apiKey) {
	(void)apiKey;
	(void)this->httpClient;
	return {};
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const std::string& apiKey) {
	(void)event;
	(void)apiKey;
	(void)this->httpClient;
	return true;
}
