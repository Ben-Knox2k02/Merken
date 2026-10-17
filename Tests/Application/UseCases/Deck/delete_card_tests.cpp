#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/DeleteCard/delete_card_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("DeleteCardUseCase removes a card from a deck") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	DeleteCardUseCase useCase(deckRepository);

	DeleteCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;

	CHECK(useCase.Execute(request));
	CHECK(deckRepository.FindStoredCard(deckId, cardId) == nullptr);
}

TEST_CASE("DeleteCardUseCase returns false when the deck is missing") {
	FakeDeckRepository deckRepository;
	DeleteCardUseCase useCase(deckRepository);

	DeleteCardRequest request;
	request.deckId = 99;
	request.cardId = 1;

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("DeleteCardUseCase returns false when the card is missing") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	DeleteCardUseCase useCase(deckRepository);

	DeleteCardRequest request;
	request.deckId = deckId;
	request.cardId = 99;

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("DeleteCardUseCase returns false when the db delete fails") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	deckRepository.deleteCardSucceeds = false;
	DeleteCardUseCase useCase(deckRepository);

	DeleteCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;

	CHECK_FALSE(useCase.Execute(request));
	CHECK(deckRepository.FindStoredCard(deckId, cardId) != nullptr);
}
