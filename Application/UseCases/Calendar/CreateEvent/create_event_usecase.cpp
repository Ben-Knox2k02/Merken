#include "create_event_usecase.h"

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
	CalendarEvent event(0, request.title, request.startTime, request.endTime);
	event.UpdateDescription(request.description);
	event.UpdateReminderMinutes(request.reminderMinutes);

	AppSettings settings = this->appSettingsService.GetSettings();
	CalendarEvent createdEvent = this->calendarAPIService.CreateEvent(event, settings.calendarApiKey);

	return CreateEventResponse{
		.calendarEventId = createdEvent.GetCalendarEventId(),
		.title = createdEvent.GetTitle(),
		.description = createdEvent.GetDescription(),
		.startTime = createdEvent.GetStartTime(),
		.endTime = createdEvent.GetEndTime(),
		.reminderMinutes = createdEvent.GetReminderMinutes(),
		.googleEventId = createdEvent.GetGoogleEventId()
	};
}
