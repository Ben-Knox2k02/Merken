#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/UpdateCard/update_card_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("UpdateCardUseCase updates front, back, and tags") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello", "Greeting");
	UpdateCardUseCase useCase(deckRepository);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.front = "Buenos dias";
	request.back = "Good morning";
	request.tags = "Greeting, Formal";

	CHECK(useCase.Execute(request));
	Card* updated = deckRepository.FindStoredCard(deckId, cardId);
	REQUIRE(updated != nullptr);
	CHECK(updated->GetFront() == "Buenos dias");
	CHECK(updated->GetBack() == "Good morning");
	REQUIRE(updated->GetTags().size() == 2);
	CHECK(updated->GetTags()[0].GetName() == "Greeting");
	CHECK(updated->GetTags()[1].GetName() == "Formal");
}

TEST_CASE("UpdateCardUseCase returns false when the deck is missing") {
	FakeDeckRepository deckRepository;
	UpdateCardUseCase useCase(deckRepository);

	UpdateCardRequest request;
	request.deckId = 99;
	request.cardId = 1;
	request.front = "Buenos dias";
	request.back = "Good morning";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateCardUseCase returns false when the card is missing") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	UpdateCardUseCase useCase(deckRepository);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = 99;
	request.front = "Buenos dias";
	request.back = "Good morning";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateCardUseCase returns false when the db update fails") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	deckRepository.updateCardSucceeds = false;
	UpdateCardUseCase useCase(deckRepository);

	UpdateCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.front = "Buenos dias";
	request.back = "Good morning";

	CHECK_FALSE(useCase.Execute(request));
}
