#include "update_event_usecase.h"
#include <optional>

bool UpdateEventUseCase::Execute(const UpdateEventRequest& request) {
	const std::string accessToken = this->authService.GetAccessToken();
	std::optional<CalendarEvent> existing = this->calendarAPIService.GetEvent(
		request.googleEventId,
		accessToken);
	if (!existing.has_value()) { return false; }

	existing->UpdateTitle(request.title);
	existing->UpdateDate(request.date);
	existing->UpdateTimeRange(request.startTime, request.endTime);
	existing->UpdateDescription(request.description);
	existing->UpdateLocation(request.location);
	existing->UpdateReminderMinutes(request.reminderMinutes);
	return this->calendarAPIService.UpdateEvent(*existing, accessToken);
}