#include "update_event_usecase.h"
#include <optional>

bool UpdateEventUseCase::Execute(const UpdateEventRequest& request) {
    CalendarEvent event(
        request.calendarEventId,
        request.title,
        request.date,
        request.startTime,
        request.endTime,
        request.location,
        request.description
    );
    event.UpdateReminderMinutes(request.reminderMinutes);

    // OVERWRITE THE LOCAL SQLITE DATABASE
    bool localSuccess = this->eventRepository.UpdateEvent(event);
    if(!localSuccess) {
        return false;
    }

    // Optional Sync with Google Calendar API
    if(!request.googleEventId.empty()) {
        const std::string accessToken = this->authService.GetAccessToken();
        std::optional<CalendarEvent> existing = this->calendarAPIService.GetEvent(
            request.googleEventId,
            accessToken);
            
        if(existing.has_value()) {
            existing->UpdateTitle(request.title);
            existing->UpdateDate(request.date);
            existing->UpdateTimeRange(request.startTime, request.endTime);
            existing->UpdateDescription(request.description);
            existing->UpdateLocation(request.location);
            existing->UpdateReminderMinutes(request.reminderMinutes);
            
            this->calendarAPIService.UpdateEvent(*existing, accessToken);
        }
    }

    return true;
}
