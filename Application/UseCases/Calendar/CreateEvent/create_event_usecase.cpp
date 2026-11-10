#include "create_event_usecase.h"
#include <exception>
#include <optional>

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
	CalendarEvent event(
		0,
		request.title,
		request.date,
		request.startTime,
		request.endTime,
		request.location,
		request.description
	);
	event.UpdateReminderMinutes(request.reminderMinutes);

	const int localId = this->eventRepository.AddEvent(event);
	std::string googleEventId;
	if (localId > 0) {
		try {
			const std::string accessToken = this->authService.GetAccessToken();
			if (!accessToken.empty()) {
				const CalendarEvent created = this->calendarAPIService.CreateEvent(event, accessToken);
				googleEventId = created.GetGoogleEventId();
				if (!googleEventId.empty()) {
					std::optional<CalendarEvent> stored = this->eventRepository.GetEvent(localId);
					if (stored.has_value()) {
						stored->SetGoogleEventId(googleEventId);
						this->eventRepository.UpdateEvent(*stored);
					}
				}
			}
		} catch (const std::exception&) {
			googleEventId.clear();
		}
	}

	return CreateEventResponse{
		.calendarEventId = localId,
		.title = request.title,
		.date = request.date,
		.description = request.description,
		.startTime = request.startTime,
		.endTime = request.endTime,
		.location = request.location,
		.reminderMinutes = request.reminderMinutes,
		.googleEventId = googleEventId
	};
}
