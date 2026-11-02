#ifndef CREATE_EVENT_RESPONSE_H
#define CREATE_EVENT_RESPONSE_H

#include "../../../../DomainModels/date.h"
#include <string>

struct CreateEventResponse {
	int calendarEventId;
	std::string title;
	Date date;
	std::string startTime;
	std::string endTime;
	std::string location;
	std::string description;
	int reminderMinutes;
	std::string googleEventId;
};

#endif
