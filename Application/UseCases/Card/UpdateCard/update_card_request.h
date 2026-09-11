#ifndef UPDATE_CARD_REQUEST_H
#define UPDATE_CARD_REQUEST_H

#include <string>

struct UpdateCardRequest {
	int cardId;
	std::string front;
	std::string back;
	std::string tags;
};

#endif
