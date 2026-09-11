#include "google_calendar_service.h"

// dummy placeholder, does not call the real Google Calendar API yet
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const AppSettings& settings) {
	(void)settings;
	(void)this->httpClient;
	return event;
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const AppSettings& settings) {
	(void)event;
	(void)settings;
	(void)this->httpClient;
	return true;
}
