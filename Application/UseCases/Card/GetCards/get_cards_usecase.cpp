#include "get_cards_usecase.h"
#include "../tag_utils.h"

GetCardsResponse GetCardsUseCase::Execute(const GetCardsRequest& request) {
	GetCardsResponse response;
	for (const Card& card : this->cardDBService.GetCards(request.deckId)) {
		response.cards.push_back(CardResponse{
			card.GetCardId(),
			card.GetDeckId(),
			card.GetFront(),
			card.GetBack(),
			JoinTags(card.GetTags())
		});
	}
	return response;
}
