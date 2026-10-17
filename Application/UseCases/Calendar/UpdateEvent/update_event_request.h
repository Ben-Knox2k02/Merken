#ifndef UPDATE_EVENT_REQUEST_H
#define UPDATE_EVENT_REQUEST_H

#include <string>

struct UpdateEventRequest {
	int calendarEventId = 0;
	std::string googleEventId;
	std::string title;
	std::string description;
	std::string startTime;
	std::string endTime;
	int reminderMinutes = 60;
};

#endif
