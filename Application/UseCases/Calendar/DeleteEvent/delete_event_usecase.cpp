#include "delete_event_usecase.h"

DeleteEventUseCase::DeleteEventUseCase(std::shared_ptr<GoogleCalendarService> calendarService)
    : calendarService_(calendarService) {}

void DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
    // Call your service/repository delete method here
    // e.g., calendarService_->DeleteEvent(request.id);
}
