#ifndef GET_CARDS_RESPONSE_H
#define GET_CARDS_RESPONSE_H

#include <vector>
#include "card_response.h"

struct GetCardsResponse {
	std::vector<CardResponse> cards;
};

#endif
