#ifndef CREATE_EVENT_RESPONSE_H
#define CREATE_EVENT_RESPONSE_H

#include <string>

struct CreateEventResponse {
	int calendarEventId;
	std::string title;
	std::string description;
	std::string startTime;
	std::string endTime;
	int reminderMinutes;
	std::string googleEventId;
};

#endif
