#ifndef GET_EVENTS_USE_CASE_H
#define GET_EVENTS_USE_CASE_H

#include "get_events_response.h"
#include "../../../Repositories/event_repository.h"

class GetEventsUseCase {
	public:
		explicit GetEventsUseCase(IEventRepository& eventRepository)
			: eventRepository(eventRepository) {}

		GetEventsResponse Execute();
		GetEventsResponse ExecuteForDate(const Date& date);

	private:
		GetEventsResponse MapEvents(const std::vector<CalendarEvent>& events);

		IEventRepository& eventRepository;
};

#endif
