#include "get_cards_usecase.h"
#include <optional>

GetCardsResponse GetCardsUseCase::Execute(const GetCardsRequest& request) {
	GetCardsResponse response;
	std::optional<Deck> deck = this->deckDBService.GetDeck(request.deckId);
	if (!deck.has_value()) {
		return response;
	}

	for (const Card& card : deck->GetCards()) {
		response.cards.push_back(CardResponse{
			.cardId = card.GetCardId(),
			.deckId = card.GetDeckId(),
			.front = card.GetFront(),
			.back = card.GetBack(),
			.tags = Tag::Join(card.GetTags())
		});
	}
	return response;
}
