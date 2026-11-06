#include "delete_card_usecase.h"
#include <optional>

bool DeleteCardUseCase::Execute(const DeleteCardRequest& request) {
	std::optional<Deck> deck = this->deckRepository.GetDeck(request.deckId);
	if (!deck.has_value()) {
		return false;
	}
	if (deck->FindCard(request.cardId) == nullptr) {
		return false;
	}
	return this->deckRepository.DeleteCard(request.deckId, request.cardId);
}
