#ifndef GET_TODAYS_PROGRESS_RESPONSE_H
#define GET_TODAYS_PROGRESS_RESPONSE_H

#include <string>

struct GetTodaysProgressResponse {
	std::string date;
	int cardsReviewed;
	int cardsCorrect;
	double retentionRate;
};

#endif
