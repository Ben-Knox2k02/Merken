#ifndef EVENT_RESPONSE_H
#define EVENT_RESPONSE_H

#include <string>
#include "../../../../DomainModels/date.h"

struct EventResponse {
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
