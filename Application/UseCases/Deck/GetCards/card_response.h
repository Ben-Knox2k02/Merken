#ifndef CARD_RESPONSE_H
#define CARD_RESPONSE_H

#include <string>

struct CardResponse {
	int cardId;
	int deckId;
	std::string front;
	std::string back;
	std::string tags;
};

#endif
