#ifndef UPDATE_DECK_REQUEST_H
#define UPDATE_DECK_REQUEST_H

#include <string>

struct UpdateDeckRequest {
	int deckId;
	std::string name;
	std::string description;
};

#endif
