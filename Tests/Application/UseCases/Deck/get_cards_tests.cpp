#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/GetCards/get_cards_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("GetCardsUseCase returns no cards when the deck is missing") {
	FakeDeckRepository deckRepository;
	GetCardsUseCase useCase(deckRepository);

	GetCardsRequest request;
	request.deckId = 99;

	GetCardsResponse response = useCase.Execute(request);

	CHECK(response.cards.empty());
}

TEST_CASE("GetCardsUseCase maps cards in a deck") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.SeedCard(deckId, "Hola", "Hello", "Greeting");
	deckRepository.SeedCard(deckId, "Adios", "Goodbye");
	GetCardsUseCase useCase(deckRepository);

	GetCardsRequest request;
	request.deckId = deckId;

	GetCardsResponse response = useCase.Execute(request);

	REQUIRE(response.cards.size() == 2);
	CHECK(response.cards[0].cardId == 1);
	CHECK(response.cards[0].deckId == deckId);
	CHECK(response.cards[0].front == "Hola");
	CHECK(response.cards[0].back == "Hello");
	CHECK(response.cards[0].tags == "Greeting");
	CHECK(response.cards[1].cardId == 2);
	CHECK(response.cards[1].front == "Adios");
	CHECK(response.cards[1].back == "Goodbye");
	CHECK(response.cards[1].tags == "");
}
