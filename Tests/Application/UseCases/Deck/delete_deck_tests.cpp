#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/DeleteDeck/delete_deck_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("DeleteDeckUseCase removes a deck and its cards") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.SeedCard(deckId, "Hola", "Hello");
	DeleteDeckUseCase useCase(deckRepository);

	DeleteDeckRequest request;
	request.deckId = deckId;

	CHECK(useCase.Execute(request));
	CHECK_FALSE(deckRepository.GetDeck(deckId).has_value());
}

TEST_CASE("DeleteDeckUseCase returns false when the deck is missing") {
	FakeDeckRepository deckRepository;
	DeleteDeckUseCase useCase(deckRepository);

	DeleteDeckRequest request;
	request.deckId = 99;

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("DeleteDeckUseCase returns false when the db delete fails") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.deleteDeckSucceeds = false;
	DeleteDeckUseCase useCase(deckRepository);

	DeleteDeckRequest request;
	request.deckId = deckId;

	CHECK_FALSE(useCase.Execute(request));
	CHECK(deckRepository.GetDeck(deckId).has_value());
}
