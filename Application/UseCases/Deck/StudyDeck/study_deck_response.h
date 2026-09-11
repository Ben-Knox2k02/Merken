#ifndef STUDY_DECK_RESPONSE_H
#define STUDY_DECK_RESPONSE_H

#include <string>
#include <vector>
#include "../../Card/GetCards/card_response.h"

struct StudyDeckResponse {
	int deckId;
	std::string deckName;
	std::vector<CardResponse> dueCards;
};

#endif
