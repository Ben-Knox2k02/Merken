#ifndef CREATE_EVENT_USE_CASE_H
#define CREATE_EVENT_USE_CASE_H

#include "create_event_request.h"
#include "create_event_response.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class CreateEventUseCase {
	public:
		CreateEventUseCase(
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : calendarAPIService(calendarAPIService),
			authService(authService) {}

		CreateEventResponse Execute(const CreateEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif