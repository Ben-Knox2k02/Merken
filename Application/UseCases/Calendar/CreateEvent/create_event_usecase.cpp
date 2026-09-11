#include "create_event_usecase.h"

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
	CalendarEvent event(0, request.title, request.startTime, request.endTime);
	event.UpdateDescription(request.description);
	event.UpdateReminderMinutes(request.reminderMinutes);

	AppSettings settings = this->appSettingsService.GetSettings();
	CalendarEvent createdEvent = this->calendarAPIService.CreateEvent(event, settings);

	return CreateEventResponse{
		createdEvent.GetCalendarEventId(),
		createdEvent.GetTitle(),
		createdEvent.GetDescription(),
		createdEvent.GetStartTime(),
		createdEvent.GetEndTime(),
		createdEvent.GetReminderMinutes(),
		createdEvent.GetGoogleEventId()
	};
}
