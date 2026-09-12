#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../../Fakes/fake_deck_db_service.h"

TEST_CASE("UpdateCardUseCase updates front, back, and tags") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	const int cardId = deckDb.SeedCard(deckId, "Hola", "Hello", "Old");
	UpdateCardUseCase useCase(deckDb);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.front = "Buenos dias";
	request.back = "Good morning";
	request.tags = "Greeting, Formal";

	CHECK(useCase.Execute(request));
	Card* updated = deckDb.FindStoredCard(deckId, cardId);
	REQUIRE(updated != nullptr);
	CHECK(updated->GetFront() == "Buenos dias");
	CHECK(updated->GetBack() == "Good morning");
	REQUIRE(updated->GetTags().size() == 2);
	CHECK(updated->GetTags()[0].GetName() == "Greeting");
	CHECK(updated->GetTags()[1].GetName() == "Formal");
}

TEST_CASE("UpdateCardUseCase returns false when the deck is missing") {
	FakeDeckDbService deckDb;
	UpdateCardUseCase useCase(deckDb);

	UpdateCardRequest request;
	request.deckId = 99;
	request.cardId = 1;
	request.front = "A";
	request.back = "B";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateCardUseCase returns false when the card is missing") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	UpdateCardUseCase useCase(deckDb);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = 99;
	request.front = "A";
	request.back = "B";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateCardUseCase returns false when the db update fails") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	const int cardId = deckDb.SeedCard(deckId, "Hola", "Hello");
	deckDb.updateCardSucceeds = false;
	UpdateCardUseCase useCase(deckDb);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.front = "A";
	request.back = "B";

	CHECK_FALSE(useCase.Execute(request));
}
