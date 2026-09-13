#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/CreateDeck/create_deck_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 12);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("CreateDeckUseCase persists a deck and returns it") {
	FakeDeckRepository deckRepository;
	FakeDateProviderService dates(TestDate());
	CreateDeckUseCase useCase(deckRepository, dates);

	CreateDeckRequest request;
	request.name = "Spanish";
	request.description = "Vocabulary";

	CreateDeckResponse response = useCase.Execute(request);

	REQUIRE(deckRepository.addedDecks.size() == 1);
	const Deck& saved = deckRepository.addedDecks[0];
	CHECK(saved.GetDeckId() == 0);
	CHECK(saved.GetName() == "Spanish");
	CHECK(saved.GetDescription() == "Vocabulary");
	CHECK(saved.GetCreatedAt() == "2026-09-12");

	CHECK(response.deckId == 1);
	CHECK(response.name == "Spanish");
	CHECK(response.description == "Vocabulary");
	CHECK(response.createdAt == "2026-09-12");
}

TEST_CASE("CreateDeckUseCase allows an empty description") {
	FakeDeckRepository deckRepository;
	FakeDateProviderService dates(TestDate());
	CreateDeckUseCase useCase(deckRepository, dates);

	CreateDeckRequest request;
	request.name = "Untitled";
	request.description = "";

	CreateDeckResponse response = useCase.Execute(request);

	REQUIRE(deckRepository.addedDecks.size() == 1);
	CHECK(deckRepository.addedDecks[0].GetDescription() == "");
	CHECK(response.description == "");
	CHECK(response.deckId == 1);
}

TEST_CASE("CreateDeckUseCase returns ids from the repository") {
	FakeDeckRepository deckRepository;
	deckRepository.nextDeckId = 42;
	FakeDateProviderService dates(TestDate());
	CreateDeckUseCase useCase(deckRepository, dates);

	CreateDeckRequest first;
	first.name = "One";
	CreateDeckRequest second;
	second.name = "Two";

	CHECK(useCase.Execute(first).deckId == 42);
	CHECK(useCase.Execute(second).deckId == 43);
	CHECK(deckRepository.addedDecks.size() == 2);
}
