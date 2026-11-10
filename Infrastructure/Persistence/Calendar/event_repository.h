#ifndef EVENT_REPOSITORY_REAL_H
#define EVENT_REPOSITORY_REAL_H

#include "../../../Application/Repositories/event_repository.h"
#include "../DatabaseContext/database_context.h"

class EventRepository : public IEventRepository {
	public:
		explicit EventRepository(DatabaseContext& dbContext) : db(dbContext) {}

		std::vector<CalendarEvent> GetEvents() override;
		std::vector<CalendarEvent> GetEventsForDate(const Date& date) override;
		std::optional<CalendarEvent> GetEvent(int calendarEventId) override;
		int AddEvent(const CalendarEvent& event) override;
		bool UpdateEvent(const CalendarEvent& event) override;
		bool DeleteEvent(int calendarEventId) override;

	private:
		DatabaseContext& db;
};

#endif
