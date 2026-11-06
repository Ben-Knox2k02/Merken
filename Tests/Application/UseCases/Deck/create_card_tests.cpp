#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/CreateCard/create_card_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("CreateCardUseCase persists a card on an existing deck") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	CreateCardUseCase useCase(deckRepository);

	CreateCardRequest request;
	request.deckId = deckId;
	request.front = "Hola";
	request.back = "Hello";
	request.tags = "Greeting, Informal";

	CreateCardResponse response = useCase.Execute(request);

	REQUIRE(deckRepository.addedCards.size() == 1);
	CHECK(deckRepository.addedCards[0].GetCardId() == 0);
	CHECK(deckRepository.addedCards[0].GetDeckId() == deckId);
	CHECK(deckRepository.addedCards[0].GetFront() == "Hola");
	CHECK(deckRepository.addedCards[0].GetBack() == "Hello");
	REQUIRE(deckRepository.addedCards[0].GetTags().size() == 2);
	CHECK(deckRepository.addedCards[0].GetTags()[0].GetName() == "Greeting");
	CHECK(deckRepository.addedCards[0].GetTags()[1].GetName() == "Informal");

	CHECK(response.cardId == 1);
	CHECK(response.deckId == deckId);
	CHECK(response.front == "Hola");
	CHECK(response.back == "Hello");
	CHECK(response.tags == "Greeting, Informal");
}

TEST_CASE("CreateCardUseCase returns cardId 0 when the deck is missing") {
	FakeDeckRepository deckRepository;
	CreateCardUseCase useCase(deckRepository);

	CreateCardRequest request;
	request.deckId = 99;
	request.front = "Hola";
	request.back = "Hello";

	CreateCardResponse response = useCase.Execute(request);

	CHECK(response.cardId == 0);
	CHECK(response.deckId == 99);
	CHECK(deckRepository.addedCards.empty());
}
