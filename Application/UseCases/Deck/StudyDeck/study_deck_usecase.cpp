#include "study_deck_usecase.h"
#include <optional>

StudyDeckResponse StudyDeckUseCase::Execute(const StudyDeckRequest& request) {
	StudyDeckResponse response;
	response.deckId = request.deckId;

	std::optional<Deck> deck = this->deckDBService.GetDeck(request.deckId);
	if (!deck.has_value()) {
		return response;
	}
	response.deckName = deck->GetName();

	const Date today = this->dateProviderService.GetCurrentDate();
	for (const Card& card : this->cardDBService.GetCards(request.deckId)) {
		deck->AddCard(card);
	}

	for (const Card& card : deck->GetDueCards(today)) {
		response.dueCards.push_back(CardResponse{
			card.GetCardId(),
			card.GetDeckId(),
			card.GetFront(),
			card.GetBack(),
			Tag::Join(card.GetTags())
		});
	}
	return response;
}
