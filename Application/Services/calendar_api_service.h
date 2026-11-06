#ifndef CALENDAR_API_SERVICE_H
#define CALENDAR_API_SERVICE_H

#include <optional>
#include <string>
#include <vector>
#include "../../DomainModels/calendar_event.h"

class ICalendarAPIService {
	public:
		virtual ~ICalendarAPIService() = default;

		virtual CalendarEvent CreateEvent(const CalendarEvent& event, const std::string& apiKey) = 0;
		virtual std::optional<CalendarEvent> GetEvent(const std::string& googleEventId, const std::string& apiKey) = 0;
		virtual std::vector<CalendarEvent> GetEvents(const std::string& apiKey) = 0;
		virtual bool UpdateEvent(const CalendarEvent& event, const std::string& apiKey) = 0;
};

#endif
