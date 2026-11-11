#include "delete_event_usecase.h"

bool DeleteEventUseCase::Execute(const DeleteEventRequest& request) {
	const std::string accessToken = this->authService.GetAccessToken();
	return this->calendarAPIService.DeleteEvent(request.id, accessToken);
}