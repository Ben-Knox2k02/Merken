#include "create_event_usecase.h"

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
    CalendarEvent event(0, request.title, request.date, request.startTime, request.endTime, request.location, request.description);
    
    event.UpdateLocation(request.location);
    event.UpdateDescription(request.description);
    event.UpdateReminderMinutes(request.reminderMinutes);

    int localDbId = this->eventRepository.AddEvent(event);

    const std::string accessToken = this->authService.GetAccessToken();
    CalendarEvent createdEvent = this->calendarAPIService.CreateEvent(event, accessToken);

    return CreateEventResponse{
        .calendarEventId = localDbId > 0 ? localDbId : createdEvent.GetCalendarEventId(),
        .title = event.GetTitle(),
        .date = event.GetDate(),
        .startTime = event.GetStartTime(),
        .endTime = event.GetEndTime(),
        .location = event.GetLocation(),        
        .description = event.GetDescription(),
        .reminderMinutes = event.GetReminderMinutes(),
        .googleEventId = createdEvent.GetGoogleEventId()
    };
}