#include "create_event_use_case.h"

CalendarEvent CreateEventUseCase::Execute(const CreateEventRequest& request) {
	CalendarEvent event(0, request.courseID, request.title, request.description, request.startTime, request.endTime);
	AppSettings settings = this->appSettingsService.GetSettings();
	return this->calendarAPIService.CreateEvent(event, settings);
}
