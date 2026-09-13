#ifndef CALENDAR_API_SERVICE_H
#define CALENDAR_API_SERVICE_H

#include <optional>
#include <string>
#include "../../DomainModels/calendar_event.h"
#include "../../DomainModels/app_settings.h"

class ICalendarAPIService {
	public:
		virtual ~ICalendarAPIService() = default;

		virtual CalendarEvent CreateEvent(const CalendarEvent& event, const AppSettings& settings) = 0;
		virtual std::optional<CalendarEvent> GetEvent(const std::string& googleEventId, const AppSettings& settings) = 0;
		virtual bool UpdateEvent(const CalendarEvent& event, const AppSettings& settings) = 0;
};

#endif
