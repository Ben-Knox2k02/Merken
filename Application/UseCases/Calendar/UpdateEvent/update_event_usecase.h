#ifndef UPDATE_EVENT_USE_CASE_H
#define UPDATE_EVENT_USE_CASE_H

#include "update_event_request.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/app_settings_service.h"

class UpdateEventUseCase {
	public:
		UpdateEventUseCase(
			ICalendarAPIService& calendarAPIService,
			IAppSettingsService& appSettingsService
		) : calendarAPIService(calendarAPIService),
			appSettingsService(appSettingsService) {}

		bool Execute(const UpdateEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		IAppSettingsService& appSettingsService;
};

#endif
