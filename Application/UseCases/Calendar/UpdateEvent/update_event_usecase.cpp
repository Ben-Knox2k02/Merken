#include "update_event_usecase.h"
#include <exception>
#include <optional>

bool UpdateEventUseCase::Execute(const UpdateEventRequest& request) {
	std::optional<CalendarEvent> existing = this->eventRepository.GetEvent(request.calendarEventId);
	if (!existing.has_value()) {
		return false;
	}

	existing->UpdateTitle(request.title);
	existing->UpdateDate(request.date);
	existing->UpdateDescription(request.description);
	existing->UpdateTimeRange(request.startTime, request.endTime);
	existing->UpdateLocation(request.location);
	existing->UpdateReminderMinutes(request.reminderMinutes);
	if (!this->eventRepository.UpdateEvent(*existing)) {
		return false;
	}

	try {
		const std::string accessToken = this->authService.GetAccessToken();
		if (!accessToken.empty() && !existing->GetGoogleEventId().empty()) {
			this->calendarAPIService.UpdateEvent(*existing, accessToken);
		}
	} catch (const std::exception&) {
		// The local row is already saved. Google is optional.
	}
	return true;
}
