#ifndef GET_EVENTS_RESPONSE_H
#define GET_EVENTS_RESPONSE_H

#include <vector>
#include "event_response.h"

struct GetEventsResponse {
	std::vector<EventResponse> events;
};

#endif
