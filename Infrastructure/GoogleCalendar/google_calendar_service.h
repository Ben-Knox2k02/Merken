#ifndef GOOGLE_CALENDAR_SERVICE_REAL_H
#define GOOGLE_CALENDAR_SERVICE_REAL_H

#include "../../Application/ServiceInterfaces/calendar_api_service.h"

// dummy placeholder, does not call the real Google Calendar API yet
class GoogleCalendarService : public ICalendarAPIService {
	public:
		CalendarEvent CreateEvent(const CalendarEvent& event, const AppSettings& settings) override;
		bool UpdateEvent(const CalendarEvent& event, const AppSettings& settings) override;
};

#endif
