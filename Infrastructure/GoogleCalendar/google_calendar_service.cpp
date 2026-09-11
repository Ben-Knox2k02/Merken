#include "google_calendar_service.h"

// dummy placeholder, does not call the real Google Calendar API yet
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const AppSettings& settings) {
	return event;
}

// dummy placeholder, does not call the real Google Calendar API yet
bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const AppSettings& settings) {
	return true;
}
