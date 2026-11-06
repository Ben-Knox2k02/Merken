#ifndef DELETE_EVENT_USECASE_H
#define DELETE_EVENT_USECASE_H

#include "../../../Repositories/event_repository.h"
#include "../../../Services/calendar_api_service.h" // or GoogleCalendarService
#include "../../../Services/calendar_auth_service.h"
#include "delete_event_request.h"
#include <memory>

class DeleteEventUseCase {
	public:
		DeleteEventUseCase(
			IEventRepository& eventRepository,
			std::shared_ptr<ICalendarAPIService> calendarService,
			ICalendarAuthService& authService
		);

		void Execute(const DeleteEventRequest& request);
	
	private:
		IEventRepository& eventRepository;
		std::shared_ptr<ICalendarAPIService> calendarService_;
		ICalendarAuthService& authService_;
};

#endif