#ifndef CREATE_EVENT_RESPONSE_H
#define CREATE_EVENT_RESPONSE_H

#include <string>
#include <wx/datetime.h>

struct CreateEventResponse {
	std::string id;
    std::string courseID;
    std::string title;
    std::string description;
    wxDateTime startTime;
    wxDateTime endTime;
};

#endif