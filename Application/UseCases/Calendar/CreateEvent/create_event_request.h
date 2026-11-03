#ifndef CREATE_EVENT_REQUEST_H
#define CREATE_EVENT_REQUEST_H

#include "../../../../DomainModels/date.h"
#include <string>

struct CreateEventRequest {
    std::string title;       	// Required
	Date date = Date::Create(2026, 1, 1).value();        			// Required 
    std::string startTime;   	// Required ("HH:MM")
    std::string endTime;     	// Required ("HH:MM")
    std::string location;    	// Optional
    std::string description; 	// Optional
    int reminderMinutes = 60;	// Optional (with default)
};

#endif  // CREATE_EVENT_REQUEST_H
