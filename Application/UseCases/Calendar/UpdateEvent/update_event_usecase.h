#ifndef UPDATE_EVENT_USE_CASE_H
#define UPDATE_EVENT_USE_CASE_H

#include "update_event_request.h"
#include "../../../Repositories/event_repository.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class UpdateEventUseCase {
	public:
		UpdateEventUseCase(
			IEventRepository& eventRepository,
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : eventRepository(eventRepository),
			calendarAPIService(calendarAPIService),
			authService(authService) {}

		bool Execute(const UpdateEventRequest& request);

	private:
		IEventRepository& eventRepository;
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif
