#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/GetDecks/get_decks_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("GetDecksUseCase returns no decks when none exist") {
	FakeDeckRepository deckRepository;
	GetDecksUseCase useCase(deckRepository);

	GetDecksResponse response = useCase.Execute();

	CHECK(response.decks.empty());
}

TEST_CASE("GetDecksUseCase maps stored decks") {
	FakeDeckRepository deckRepository;
	deckRepository.SeedDeck("Spanish", "Vocabulary", "2026-09-12");
	deckRepository.SeedDeck("History", "", "2026-01-01");
	GetDecksUseCase useCase(deckRepository);

	GetDecksResponse response = useCase.Execute();

	REQUIRE(response.decks.size() == 2);
	CHECK(response.decks[0].deckId == 1);
	CHECK(response.decks[0].name == "Spanish");
	CHECK(response.decks[0].description == "Vocabulary");
	CHECK(response.decks[0].createdAt == "2026-09-12");
	CHECK(response.decks[1].deckId == 2);
	CHECK(response.decks[1].name == "History");
	CHECK(response.decks[1].description == "");
	CHECK(response.decks[1].createdAt == "2026-01-01");
}
