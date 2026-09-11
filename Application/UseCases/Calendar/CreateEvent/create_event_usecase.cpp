#include "create_event_usecase.h"

CreateEventResponse CreateEventUseCase::Execute(const CreateEventRequest& request) {
	std::string newEventId = this->uuidGeneratorService.GenerateUuid();
	CalendarEvent event(
		newEventId,
		request.courseID,
		request.title,
		request.description,
		request.startTime,
		request.endTime);
		
	AppSettings settings = this->appSettingsService.GetSettings();
	CalendarEvent createdEvent = this->calendarAPIService.CreateEvent(event, settings);

	return CreateEventResponse{
		createdEvent.GetId(),
		createdEvent.GetCourseID(),
		createdEvent.GetTitle(),
		createdEvent.GetDescription(),
		createdEvent.GetStartTime(),
		createdEvent.GetEndTime()
	};
}
