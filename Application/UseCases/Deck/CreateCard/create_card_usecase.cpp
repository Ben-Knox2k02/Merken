#include "create_card_usecase.h"
#include <optional>

CreateCardResponse CreateCardUseCase::Execute(const CreateCardRequest& request) {
	CreateCardResponse response;
	response.cardId = 0;
	response.deckId = request.deckId;
	response.front = request.front;
	response.back = request.back;
	response.tags = request.tags;

	std::optional<Deck> deck = this->deckDBService.GetDeck(request.deckId);
	if (!deck.has_value()) {
		return response;
	}

	Card card(0, request.deckId, request.front, request.back);
	card.UpdateTags(Tag::Split(request.tags));
	deck->AddCard(card);
	response.cardId = this->deckDBService.AddCard(card);
	return response;
}
