#ifndef EVENT_REPOSITORY_REAL_H
#define EVENT_REPOSITORY_REAL_H

#include "../../../Application/Repositories/event_repository.h" // Your IEventRepository interface
#include "../DatabaseContext/database_context.h"

class EventRepository : public IEventRepository {
public:
    EventRepository(DatabaseContext& dbContext) : db(dbContext) {}

    std::vector<CalendarEvent> GetEventsForDate(const Date& date) override;
    std::optional<CalendarEvent> GetEvent(int calendarEventId) override;
	std::vector<CalendarEvent> GetEvents() override;
    int AddEvent(const CalendarEvent& event) override;
    bool UpdateEvent(const CalendarEvent& event) override;
    bool DeleteEvent(int calendarEventId) override;

private:
    DatabaseContext& db;
};

#endif