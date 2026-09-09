#ifndef CREATE_EVENT_USE_CASE_H
#define CREATE_EVENT_USE_CASE_H

#include "create_event_request.h"
#include "../../Services/calendar_api_service.h"
#include "../../Services/app_settings_service.h"

class CreateEventUseCase {
	public:
		CreateEventUseCase(
			ICalendarAPIService& calendarAPIService,
			IAppSettingsService& appSettingsService
		) : calendarAPIService(calendarAPIService), appSettingsService(appSettingsService) {}

		CalendarEvent Execute(const CreateEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		IAppSettingsService& appSettingsService;
};

#endif
