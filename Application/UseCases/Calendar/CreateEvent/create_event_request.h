#ifndef CREATE_EVENT_REQUEST_H
#define CREATE_EVENT_REQUEST_H

#include <string>
#include <wx/datetime.h>

struct CreateEventRequest {
	std::string courseID;
	std::string title;
	std::string description;
	wxDateTime startTime;
	wxDateTime endTime;
};

#endif
