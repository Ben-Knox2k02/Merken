#include "get_events_usecase.h"

GetEventsResponse GetEventsUseCase::Execute() {
	GetEventsResponse response;
	const std::string apiKey = this->appSettingsService.GetSettings().calendarApiKey;
	for (const CalendarEvent& event : this->calendarAPIService.GetEvents(apiKey)) {
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
