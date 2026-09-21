#include "get_decks_usecase.h"

GetDecksResponse GetDecksUseCase::Execute() {
	GetDecksResponse response;
	for (const Deck& deck : this->deckRepository.GetDecks()) {
		response.decks.push_back(DeckResponse{
			.deckId = deck.GetDeckId(),
			.name = deck.GetName(),
			.description = deck.GetDescription(),
			.createdAt = deck.GetCreatedAt(),
			.cardCount = static_cast<int>(deck.GetCards().size())
		});
	}
	return response;
}
