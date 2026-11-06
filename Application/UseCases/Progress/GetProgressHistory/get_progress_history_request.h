#ifndef GET_PROGRESS_HISTORY_REQUEST_H
#define GET_PROGRESS_HISTORY_REQUEST_H

#include <string>

struct GetProgressHistoryRequest {
	std::string startDate;
	std::string endDate;
};

#endif
