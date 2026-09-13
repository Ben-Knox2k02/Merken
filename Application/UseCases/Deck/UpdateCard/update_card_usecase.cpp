#include "update_card_usecase.h"
#include <optional>

bool UpdateCardUseCase::Execute(const UpdateCardRequest& request) {
	std::optional<Deck> deck = this->deckRepository.GetDeck(request.deckId);
	if (!deck.has_value()) { return false; }

	Card* card = deck->FindCard(request.cardId);
	if (card == nullptr) { return false; }

	card->UpdateContent(request.front, request.back);
	card->UpdateTags(Tag::Split(request.tags));
	return this->deckRepository.UpdateCard(*card);
}
