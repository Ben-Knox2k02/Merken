#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/UpdateDeck/update_deck_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"

TEST_CASE("UpdateDeckUseCase updates name and description") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Old", "Old desc", "2026-09-12");
	UpdateDeckUseCase useCase(deckRepository);

	UpdateDeckRequest request;
	request.deckId = deckId;
	request.name = "New";
	request.description = "New desc";

	CHECK(useCase.Execute(request));
	std::optional<Deck> updated = deckRepository.GetDeck(deckId);
	REQUIRE(updated.has_value());
	CHECK(updated->GetName() == "New");
	CHECK(updated->GetDescription() == "New desc");
	CHECK(updated->GetCreatedAt() == "2026-09-12");
}

TEST_CASE("UpdateDeckUseCase returns false when the deck is missing") {
	FakeDeckRepository deckRepository;
	UpdateDeckUseCase useCase(deckRepository);

	UpdateDeckRequest request;
	request.deckId = 99;
	request.name = "New";
	request.description = "";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateDeckUseCase returns false when the db update fails") {
	FakeDeckRepository deckRepository;
	const int deckId = deckRepository.SeedDeck("Spanish");
	deckRepository.updateDeckSucceeds = false;
	UpdateDeckUseCase useCase(deckRepository);

	UpdateDeckRequest request;
	request.deckId = deckId;
	request.name = "New";
	request.description = "";

	CHECK_FALSE(useCase.Execute(request));
}
