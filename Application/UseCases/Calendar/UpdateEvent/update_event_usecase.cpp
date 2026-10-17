#include "update_event_usecase.h"
#include <optional>

bool UpdateEventUseCase::Execute(const UpdateEventRequest& request) {
	const std::string apiKey = this->appSettingsService.GetSettings().calendarApiKey;
	std::optional<CalendarEvent> existing = this->calendarAPIService.GetEvent(
		request.googleEventId,
		apiKey);
	if (!existing.has_value()) { return false; }

	existing->UpdateTitle(request.title);
	existing->UpdateDescription(request.description);
	existing->UpdateTimeRange(request.startTime, request.endTime);
	existing->UpdateReminderMinutes(request.reminderMinutes);
	return this->calendarAPIService.UpdateEvent(*existing, apiKey);
}
