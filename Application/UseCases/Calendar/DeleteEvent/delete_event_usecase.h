#ifndef DELETE_EVENT_USECASE_H
#define DELETE_EVENT_USECASE_H

#include "delete_event_request.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class DeleteEventUseCase {
	public:
		DeleteEventUseCase(
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : calendarAPIService(calendarAPIService),
			authService(authService) {}

		bool Execute(const DeleteEventRequest& request);

	private:
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif