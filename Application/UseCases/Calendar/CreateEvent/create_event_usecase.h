#ifndef CREATE_EVENT_USE_CASE_H
#define CREATE_EVENT_USE_CASE_H

#include "create_event_request.h"
#include "create_event_response.h"
#include "../../../Repositories/event_repository.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class CreateEventUseCase {
	public:
		CreateEventUseCase(
			IEventRepository& eventRepository,
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : eventRepository(eventRepository),
			calendarAPIService(calendarAPIService),
			authService(authService) {}

		CreateEventResponse Execute(const CreateEventRequest& request);

	private:
		IEventRepository& eventRepository;
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif
