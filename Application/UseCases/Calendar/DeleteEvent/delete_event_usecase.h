#ifndef DELETE_EVENT_USE_CASE_H
#define DELETE_EVENT_USE_CASE_H

#include "delete_event_request.h"
#include "delete_event_response.h"
#include "../../../Repositories/event_repository.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class DeleteEventUseCase {
	public:
		DeleteEventUseCase(
			IEventRepository& eventRepository,
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : eventRepository(eventRepository),
			calendarAPIService(calendarAPIService),
			authService(authService) {}

		DeleteEventResponse Execute(const DeleteEventRequest& request);

	private:
		IEventRepository& eventRepository;
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif
