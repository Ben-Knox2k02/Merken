#include "update_event_usecase.h"
#include <optional>

bool UpdateEventUseCase::Execute(const UpdateEventRequest& request) {
	AppSettings settings = this->appSettingsService.GetSettings();
	std::optional<CalendarEvent> existing = this->calendarAPIService.GetEvent(
		request.googleEventId,
		settings);
	if (!existing.has_value()) { return false; }

	existing->UpdateTitle(request.title);
	existing->UpdateDescription(request.description);
	existing->UpdateTimeRange(request.startTime, request.endTime);
	existing->UpdateReminderMinutes(request.reminderMinutes);
	return this->calendarAPIService.UpdateEvent(*existing, settings);
}
