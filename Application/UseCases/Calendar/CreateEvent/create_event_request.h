#ifndef CREATE_EVENT_REQUEST_H
#define CREATE_EVENT_REQUEST_H

#include <string>
#include "../../../../DomainModels/date.h"

struct CreateEventRequest {
	std::string title;
	Date date = Date::Create(1970, 1, 1).value();
	std::string description;
	std::string startTime;
	std::string endTime;
	std::string location;
	int reminderMinutes = 60;
};

#endif
