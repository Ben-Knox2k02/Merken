#ifndef DECK_RESPONSE_H
#define DECK_RESPONSE_H

#include <string>

struct DeckResponse {
	int deckId;
	std::string name;
	std::string description;
	std::string createdAt;
};

#endif
