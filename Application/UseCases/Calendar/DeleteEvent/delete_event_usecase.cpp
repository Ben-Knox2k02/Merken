#include "delete_event_usecase.h"
#include <wx/log.h>

bool DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
    // Delete from local SQLite database using the true event ID
    bool success = this->eventRepository.DeleteEvent(request.id);
    if(!success) {
        wxLogDebug("DeleteEventUseCase Warning: Failed to delete event ID %d from SQLite", request.id);
		return false;
    }

    // (Optional) Sync deletion with cloud API if applicable
	const std::string accessToken = this->authService.GetAccessToken();
	this->calendarAPIService.DeleteEvent(request.googleEventId, accessToken);
	
	return true;
}
