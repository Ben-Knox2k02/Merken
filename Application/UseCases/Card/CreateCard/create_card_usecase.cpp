#include "create_card_usecase.h"
#include "../tag_utils.h"
#include "../../../../DomainModels/card.h"

CreateCardResponse CreateCardUseCase::Execute(const CreateCardRequest& request) {
	Card card(0, request.deckId, request.front, request.back);
	card.UpdateTags(SplitTags(request.tags));
	int cardId = this->cardDBService.AddCard(card);
	return CreateCardResponse{cardId, request.deckId, request.front, request.back, request.tags};
}
