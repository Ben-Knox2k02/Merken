#include "get_decks_usecase.h"

GetDecksResponse GetDecksUseCase::Execute() {
	GetDecksResponse response;
	for (const Deck& deck : this->deckDBService.GetDecks()) {
		response.decks.push_back(DeckResponse{
			deck.GetDeckId(),
			deck.GetName(),
			deck.GetDescription(),
			deck.GetCreatedAt()
		});
	}
	return response;
}
