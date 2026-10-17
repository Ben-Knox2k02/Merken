#ifndef GET_PROGRESS_HISTORY_RESPONSE_H
#define GET_PROGRESS_HISTORY_RESPONSE_H

#include <string>
#include <vector>

struct DailyProgressResponse {
	std::string date;
	int cardsReviewed;
	int cardsCorrect;
	double retentionRate;
};

struct GetProgressHistoryResponse {
	std::vector<DailyProgressResponse> days;
};

#endif
