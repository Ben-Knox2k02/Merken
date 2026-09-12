#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/UpdateDeck/update_deck_usecase.h"
#include "../../../Fakes/fake_deck_db_service.h"

TEST_CASE("UpdateDeckUseCase updates name and description") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Old", "Old desc", "2026-09-12");
	UpdateDeckUseCase useCase(deckDb);

	UpdateDeckRequest request;
	request.deckId = deckId;
	request.name = "New";
	request.description = "New desc";

	CHECK(useCase.Execute(request));
	std::optional<Deck> updated = deckDb.GetDeck(deckId);
	REQUIRE(updated.has_value());
	CHECK(updated->GetName() == "New");
	CHECK(updated->GetDescription() == "New desc");
	CHECK(updated->GetCreatedAt() == "2026-09-12");
}

TEST_CASE("UpdateDeckUseCase returns false when the deck is missing") {
	FakeDeckDbService deckDb;
	UpdateDeckUseCase useCase(deckDb);

	UpdateDeckRequest request;
	request.deckId = 99;
	request.name = "New";
	request.description = "";

	CHECK_FALSE(useCase.Execute(request));
}

TEST_CASE("UpdateDeckUseCase returns false when the db update fails") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	deckDb.updateDeckSucceeds = false;
	UpdateDeckUseCase useCase(deckDb);

	UpdateDeckRequest request;
	request.deckId = deckId;
	request.name = "New";
	request.description = "";

	CHECK_FALSE(useCase.Execute(request));
}
