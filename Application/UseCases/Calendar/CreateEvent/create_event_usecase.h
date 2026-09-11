#ifndef CREATE_EVENT_USE_CASE_H
#define CREATE_EVENT_USE_CASE_H

#include "create_event_request.h"
#include "create_event_response.h"
#include "../../../ServiceInterfaces/calendar_api_service.h"
#include "../../../ServiceInterfaces/app_settings_service.h"

class CreateEventUseCase {
	public:
		CreateEventUseCase(
			ICalendarAPIService& calendarAPIService,
			IAppSettingsService& appSettingsService
		) : calendarAPIService(calendarAPIService),
			appSettingsService(appSettingsService) {}

		CreateEventResponse Execute(const CreateEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		IAppSettingsService& appSettingsService;
};

#endif
