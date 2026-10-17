#ifndef CREATE_CARD_REQUEST_H
#define CREATE_CARD_REQUEST_H

#include <string>

struct CreateCardRequest {
	int deckId;
	std::string front;
	std::string back;
	std::string tags;
};

#endif
