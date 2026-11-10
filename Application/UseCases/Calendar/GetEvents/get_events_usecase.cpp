#include "get_events_usecase.h"

GetEventsResponse GetEventsUseCase::MapEvents(const std::vector<CalendarEvent>& events) {
	GetEventsResponse response;
	for (const CalendarEvent& event : events) {
		response.events.push_back(EventResponse{
			.calendarEventId = event.GetCalendarEventId(),
			.title = event.GetTitle(),
			.date = event.GetDate(),
			.description = event.GetDescription(),
			.startTime = event.GetStartTime(),
			.endTime = event.GetEndTime(),
			.location = event.GetLocation(),
			.reminderMinutes = event.GetReminderMinutes(),
			.googleEventId = event.GetGoogleEventId()
		});
	}
	return response;
}

GetEventsResponse GetEventsUseCase::Execute() {
	return this->MapEvents(this->eventRepository.GetEvents());
}

GetEventsResponse GetEventsUseCase::ExecuteForDate(const Date& date) {
	return this->MapEvents(this->eventRepository.GetEventsForDate(date));
}
