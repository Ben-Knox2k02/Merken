#ifndef CREATE_EVENT_REQUEST_H
#define CREATE_EVENT_REQUEST_H

#include <string>

struct CreateEventRequest {
	std::string title;
	std::string description;
	std::string startTime;
	std::string endTime;
	int reminderMinutes = 60;
};

#endif
