#ifndef CREATE_DECK_RESPONSE_H
#define CREATE_DECK_RESPONSE_H

#include <string>

struct CreateDeckResponse {
	int deckId;
	std::string name;
	std::string description;
	std::string createdAt;
};

#endif
