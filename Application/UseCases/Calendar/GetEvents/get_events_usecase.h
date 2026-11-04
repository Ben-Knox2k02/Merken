#ifndef GET_EVENTS_USE_CASE_H
#define GET_EVENTS_USE_CASE_H

#include "get_events_response.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class GetEventsUseCase {
	public:
		GetEventsUseCase(
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : calendarAPIService(calendarAPIService),
			authService(authService) {}

		GetEventsResponse Execute();

	private:
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif