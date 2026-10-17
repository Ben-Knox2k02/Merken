#ifndef GET_EVENTS_USE_CASE_H
#define GET_EVENTS_USE_CASE_H

#include "get_events_response.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/app_settings_service.h"

class GetEventsUseCase {
	public:
		GetEventsUseCase(
			ICalendarAPIService& calendarAPIService,
			IAppSettingsService& appSettingsService
		) : calendarAPIService(calendarAPIService),
			appSettingsService(appSettingsService) {}

		GetEventsResponse Execute();

	private:
		ICalendarAPIService& calendarAPIService;
		IAppSettingsService& appSettingsService;
};

#endif
