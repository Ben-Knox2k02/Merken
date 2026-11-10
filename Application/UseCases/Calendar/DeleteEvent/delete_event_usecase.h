#ifndef DELETE_EVENT_USE_CASE_H
#define DELETE_EVENT_USE_CASE_H

#include "delete_event_request.h"
#include "../../../Repositories/event_repository.h"

class DeleteEventUseCase {
	public:
		explicit DeleteEventUseCase(IEventRepository& eventRepository)
			: eventRepository(eventRepository) {}

		bool Execute(const DeleteEventRequest& request);

	private:
		IEventRepository& eventRepository;
};

#endif
