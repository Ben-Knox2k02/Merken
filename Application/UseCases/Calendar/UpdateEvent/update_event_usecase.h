#ifndef UPDATE_EVENT_USE_CASE_H
#define UPDATE_EVENT_USE_CASE_H

#include "update_event_request.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class UpdateEventUseCase {
	public:
		UpdateEventUseCase(
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : calendarAPIService(calendarAPIService),
			authService(authService) {}

		bool Execute(const UpdateEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif