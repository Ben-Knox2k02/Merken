#include "get_events_usecase.h"

GetEventsResponse GetEventsUseCase::Execute() {
	GetEventsResponse response;
	
	// 1. FETCH FROM LOCAL SQLITE DATABASE REPOSITORY
	// (Assuming you add a GetEvents() method to fetch all rows, or pass a specific Date)
	std::vector<CalendarEvent> localEvents = this->eventRepository.GetEvents(); 

	for (const CalendarEvent& event : localEvents) {
		response.events.push_back(EventResponse{
			.calendarEventId = event.GetCalendarEventId(),
			.title = event.GetTitle(),
			.date = event.GetDate(),
			.startTime = event.GetStartTime(),
			.endTime = event.GetEndTime(),
			.location = event.GetLocation(),
			.description = event.GetDescription(),
			.reminderMinutes = event.GetReminderMinutes(),
			.googleEventId = event.GetGoogleEventId()
		});
	}
	return response;
}