#ifndef UPDATE_EVENT_REQUEST_H
#define UPDATE_EVENT_REQUEST_H

#include "../../../../DomainModels/date.h"
#include <string>

struct UpdateEventRequest {
	int calendarEventId;
	std::string googleEventId;
	std::string title;
	Date date = Date::Create(2026, 1, 1).value();
	std::string startTime;
	std::string endTime;
	std::string location;
	std::string description;
	int reminderMinutes = 60;
};

#endif
