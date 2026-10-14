#include "study_deck_usecase.h"
#include <optional>

StudyDeckResponse StudyDeckUseCase::Execute(const StudyDeckRequest& request) {
	StudyDeckResponse response;
	response.deckId = request.deckId;

	std::optional<std::string> deckName = this->deckRepository.GetDeckName(request.deckId);
	if (!deckName.has_value()) {
		return response;
	}
	response.deckName = *deckName;

	const Date today = this->dateProviderService.GetCurrentDate();
	for (const Card& card : this->deckRepository.GetDueCards(request.deckId, today)) {
		response.dueCards.push_back(CardResponse{
			.cardId = card.GetCardId(),
			.deckId = card.GetDeckId(),
			.front = card.GetFront(),
			.back = card.GetBack(),
			.tags = Tag::Join(card.GetTags())
		});
	}
	return response;
}
