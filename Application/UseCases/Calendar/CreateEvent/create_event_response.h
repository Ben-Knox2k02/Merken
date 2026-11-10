#ifndef CREATE_EVENT_RESPONSE_H
#define CREATE_EVENT_RESPONSE_H

#include <string>
#include "../../../../DomainModels/date.h"

struct CreateEventResponse {
	int calendarEventId;
	std::string title;
	Date date;
	std::string description;
	std::string startTime;
	std::string endTime;
	std::string location;
	int reminderMinutes;
	std::string googleEventId;
};

#endif
