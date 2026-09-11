#ifndef GET_DECKS_RESPONSE_H
#define GET_DECKS_RESPONSE_H

#include <vector>
#include "deck_response.h"

struct GetDecksResponse {
	std::vector<DeckResponse> decks;
};

#endif
