#include "delete_event_usecase.h"
#include <exception>

DeleteEventResponse DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
	DeleteEventResponse response;
	const std::optional<CalendarEvent> existing = this->eventRepository.GetEvent(request.calendarEventId);
	if (!existing.has_value()) {
		return response;
	}
	const std::string googleEventId = existing->GetGoogleEventId();
	if (!this->eventRepository.DeleteEvent(request.calendarEventId)) {
		return response;
	}
	response.deleted = true;
	if (googleEventId.empty()) {
		return response;
	}

	try {
		const std::string accessToken = this->authService.GetAccessToken();
		if (accessToken.empty() || !this->calendarAPIService.DeleteEvent(googleEventId, accessToken)) {
			response.googleFailed = true;
		}
	} catch (const std::exception&) {
		response.googleFailed = true;
	}
	return response;
}
