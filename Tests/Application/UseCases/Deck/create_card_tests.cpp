#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../../Fakes/fake_deck_db_service.h"

TEST_CASE("CreateCardUseCase persists a card on an existing deck") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	CreateCardUseCase useCase(deckDb);

	CreateCardRequest request;
	request.deckId = deckId;
	request.front = "Hola";
	request.back = "Hello";
	request.tags = "Greeting, Noun";

	CreateCardResponse response = useCase.Execute(request);

	REQUIRE(deckDb.addedCards.size() == 1);
	CHECK(deckDb.addedCards[0].GetCardId() == 0);
	CHECK(deckDb.addedCards[0].GetDeckId() == deckId);
	CHECK(deckDb.addedCards[0].GetFront() == "Hola");
	CHECK(deckDb.addedCards[0].GetBack() == "Hello");
	REQUIRE(deckDb.addedCards[0].GetTags().size() == 2);
	CHECK(deckDb.addedCards[0].GetTags()[0].GetName() == "Greeting");
	CHECK(deckDb.addedCards[0].GetTags()[1].GetName() == "Noun");

	CHECK(response.cardId == 1);
	CHECK(response.deckId == deckId);
	CHECK(response.front == "Hola");
	CHECK(response.back == "Hello");
	CHECK(response.tags == "Greeting, Noun");
}

TEST_CASE("CreateCardUseCase returns cardId 0 when the deck is missing") {
	FakeDeckDbService deckDb;
	CreateCardUseCase useCase(deckDb);

	CreateCardRequest request;
	request.deckId = 99;
	request.front = "Hola";
	request.back = "Hello";

	CreateCardResponse response = useCase.Execute(request);

	CHECK(response.cardId == 0);
	CHECK(response.deckId == 99);
	CHECK(deckDb.addedCards.empty());
}
