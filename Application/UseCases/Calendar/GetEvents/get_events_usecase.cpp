#include "get_events_usecase.h"

GetEventsResponse GetEventsUseCase::Execute() {
	GetEventsResponse response;
	const std::string apiKey = this->appSettingsService.GetSettings().calendarApiKey;
	for (const CalendarEvent& event : this->calendarAPIService.GetEvents(apiKey)) {
		response.events.push_back(EventResponse{
			.calendarEventId = event.GetCalendarEventId(),
			.title = event.GetTitle(),
			.description = event.GetDescription(),
			.startTime = event.GetStartTime(),
			.endTime = event.GetEndTime(),
			.reminderMinutes = event.GetReminderMinutes(),
			.googleEventId = event.GetGoogleEventId()
		});
	}
	return response;
}
