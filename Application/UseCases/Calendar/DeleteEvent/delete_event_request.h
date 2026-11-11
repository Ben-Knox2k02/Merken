#ifndef DELETE_EVENT_REQUEST_H
#define DELETE_EVENT_REQUEST_H

#include <string>

struct DeleteEventRequest {
    int id;						// local DB ID
	std::string googleEventId;	// Google ID	
};

#endif
