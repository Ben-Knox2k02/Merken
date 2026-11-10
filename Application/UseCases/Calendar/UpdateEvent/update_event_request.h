#ifndef UPDATE_EVENT_REQUEST_H
#define UPDATE_EVENT_REQUEST_H

#include <string>
#include "../../../../DomainModels/date.h"

struct UpdateEventRequest {
	int calendarEventId = 0;
	std::string title;
	Date date = Date::Create(1970, 1, 1).value();
	std::string description;
	std::string startTime;
	std::string endTime;
	std::string location;
	int reminderMinutes = 60;
};

#endif
