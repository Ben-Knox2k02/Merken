#include "create_deck_usecase.h"
#include "../../../../DomainModels/deck.h"

CreateDeckResponse CreateDeckUseCase::Execute(const CreateDeckRequest& request) {
	const std::string createdAt = this
		->dateProviderService
		.GetCurrentDate()
		.ToIso();
	
	Deck deck(0, request.name, request.description, createdAt);
	int deckId = this->deckDBService.AddDeck(deck);
	return CreateDeckResponse{
		deckId,
		request.name,
		request.description,
		createdAt};
}
