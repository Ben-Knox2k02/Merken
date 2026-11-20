#ifndef IMPORT_GOOGLE_EVENTS_USE_CASE_H
#define IMPORT_GOOGLE_EVENTS_USE_CASE_H

#include "import_google_events_response.h"
#include "../../../Repositories/event_repository.h"
#include "../../../Services/calendar_api_service.h"
#include "../../../Services/calendar_auth_service.h"

class ImportGoogleEventsUseCase {
	public:
		ImportGoogleEventsUseCase(
			IEventRepository& eventRepository,
			ICalendarAPIService& calendarAPIService,
			ICalendarAuthService& authService
		) : eventRepository(eventRepository),
			calendarAPIService(calendarAPIService),
			authService(authService) {}

		ImportGoogleEventsResponse Execute();

	private:
		IEventRepository& eventRepository;
		ICalendarAPIService& calendarAPIService;
		ICalendarAuthService& authService;
};

#endif
