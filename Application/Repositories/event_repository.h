#ifndef EVENT_REPOSITORY_H
#define EVENT_REPOSITORY_H

#include <optional>
#include <vector>
#include "../../DomainModels/calendar_event.h"
#include "../../DomainModels/date.h"

class IEventRepository {
	public:
		virtual ~IEventRepository() = default;

		virtual std::vector<CalendarEvent> GetEvents() = 0;
		virtual std::vector<CalendarEvent> GetEventsForDate(const Date& date) = 0;
		virtual std::optional<CalendarEvent> GetEvent(int calendarEventId) = 0;
		virtual int AddEvent(const CalendarEvent& event) = 0;
		virtual bool UpdateEvent(const CalendarEvent& event) = 0;
		virtual bool DeleteEvent(int calendarEventId) = 0;
};

#endif
