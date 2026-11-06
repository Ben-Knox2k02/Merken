#ifndef CREATE_CARD_RESPONSE_H
#define CREATE_CARD_RESPONSE_H

#include <string>

struct CreateCardResponse {
	int cardId;
	int deckId;
	std::string front;
	std::string back;
	std::string tags;
};

#endif
