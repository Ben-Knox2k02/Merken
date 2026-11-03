#include "create_event_usecase.h"

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
    CalendarEvent event(0, request.title, request.date, request.startTime, request.endTime, request.location, request.description);
    
    event.UpdateLocation(request.location);
    event.UpdateDescription(request.description);
    event.UpdateReminderMinutes(request.reminderMinutes);

    AppSettings settings = this->appSettingsService.GetSettings();
    CalendarEvent createdEvent = this->calendarAPIService.CreateEvent(event, settings.calendarApiKey);

    return CreateEventResponse{
        .calendarEventId = createdEvent.GetCalendarEventId(),
        .title = createdEvent.GetTitle(),
		.date = createdEvent.GetDate(),
		.startTime = createdEvent.GetStartTime(),
        .endTime = createdEvent.GetEndTime(),
        .location = createdEvent.GetLocation(),       
        .description = createdEvent.GetDescription(),
        .reminderMinutes = createdEvent.GetReminderMinutes(),
        .googleEventId = createdEvent.GetGoogleEventId()
    };
}
