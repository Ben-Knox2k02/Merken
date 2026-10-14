#ifndef GET_REVIEW_HISTORY_RESPONSE_H
#define GET_REVIEW_HISTORY_RESPONSE_H

#include "../../../../DomainModels/review_event.h"
#include <vector>

struct GetReviewHistoryResponse {
	std::vector<ReviewEvent> events;
};

#endif
