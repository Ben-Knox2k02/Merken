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
			card.GetCardId(),
			card.GetDeckId(),
			card.GetFront(),
			card.GetBack(),
			Tag::Join(card.GetTags())
		});
	}
	return response;
}
