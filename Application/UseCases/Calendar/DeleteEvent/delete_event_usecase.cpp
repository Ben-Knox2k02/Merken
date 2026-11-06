#include "delete_event_usecase.h"
#include <wx/log.h>

DeleteEventUseCase::DeleteEventUseCase(
    IEventRepository& eventRepository,
    std::shared_ptr<ICalendarAPIService> calendarService,
    ICalendarAuthService& authService
) : eventRepository(eventRepository),
    calendarService_(calendarService),
    authService_(authService) {}

void DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
    // 1. Delete from local SQLite database using the true event ID
    bool success = this->eventRepository.DeleteEvent(request.id);
    if(!success) {
        wxLogDebug("DeleteEventUseCase Warning: Failed to delete event ID %d from SQLite", request.id);
    }

    // 2. (Optional) Sync deletion with cloud API if applicable
    // const std::string accessToken = this->authService_.GetAccessToken();
    // this->calendarService_->DeleteEvent(request.googleEventId, accessToken);
}