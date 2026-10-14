#ifndef GET_REVIEW_HISTORY_REQUEST_H
#define GET_REVIEW_HISTORY_REQUEST_H

#include "../../../../DomainModels/date.h"
#include <optional>

struct GetReviewHistoryRequest {
	Date startDate;
	Date endDate;
	std::optional<int> deckId;
	std::optional<int> cardId;
};

#endif
