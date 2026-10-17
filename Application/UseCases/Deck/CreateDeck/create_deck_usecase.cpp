#include "create_deck_usecase.h"
#include "../../../../DomainModels/deck.h"

CreateDeckResponse CreateDeckUseCase::Execute(const CreateDeckRequest& request) {
	const std::string createdAt = this
		->dateProviderService
		.GetCurrentDate()
		.ToIso();
	
	Deck deck(0, request.name, request.description, createdAt);
	int deckId = this->deckRepository.AddDeck(deck);
	
	return CreateDeckResponse{
		.deckId = deckId,
		.name = request.name,
		.description = request.description,
		.createdAt = createdAt};
}
