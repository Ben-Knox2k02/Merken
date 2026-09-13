#ifndef EVENT_RESPONSE_H
#define EVENT_RESPONSE_H

#include <string>

struct EventResponse {
	int calendarEventId;
	std::string title;
	std::string description;
	std::string startTime;
	std::string endTime;
	int reminderMinutes;
	std::string googleEventId;
};

#endif
