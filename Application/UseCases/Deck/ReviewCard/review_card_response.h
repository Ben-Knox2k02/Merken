#ifndef REVIEW_CARD_RESPONSE_H
#define REVIEW_CARD_RESPONSE_H

#include <string>

struct ReviewCardResponse {
	bool success;
	int cardId;
	int intervalDays;
	std::string nextReviewDate;
	int cardsReviewed;
	int cardsCorrect;
	double retentionRate;
};

#endif
