#include "create_deck_usecase.h"
#include "../../../../DomainModels/deck.h"
#include <ctime>

namespace {
std::string CurrentDateIso() {
	std::time_t now = std::time(nullptr);
	std::tm* local = std::localtime(&now);
	char buf[11] = {0};
	if (local != nullptr) {
		std::strftime(buf, sizeof(buf), "%Y-%m-%d", local);
	}
	return buf;
}
}

CreateDeckResponse CreateDeckUseCase::Execute(const CreateDeckRequest& request) {
	const std::string createdAt = CurrentDateIso();
	Deck deck(0, request.name, request.description, createdAt);
	int deckId = this->deckDBService.AddDeck(deck);
	return CreateDeckResponse{deckId, request.name, request.description, createdAt};
}
