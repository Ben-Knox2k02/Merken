#include "delete_event_usecase.h"

bool DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
	return this->eventRepository.DeleteEvent(request.calendarEventId);
}
