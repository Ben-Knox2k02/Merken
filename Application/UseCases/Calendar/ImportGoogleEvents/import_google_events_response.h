#ifndef IMPORT_GOOGLE_EVENTS_RESPONSE_H
#define IMPORT_GOOGLE_EVENTS_RESPONSE_H

struct ImportGoogleEventsResponse {
	int imported = 0;
	int updated = 0;
	bool googleFailed = false;
};

#endif
