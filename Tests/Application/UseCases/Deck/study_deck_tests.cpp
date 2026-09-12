#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/StudyDeck/study_deck_usecase.h"
#include "../../../Fakes/fake_deck_db_service.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 12);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("StudyDeckUseCase returns only due cards") {
	FakeDeckDbService deckDb;
	const int deckId = deckDb.SeedDeck("Spanish");
	const int dueNew = deckDb.SeedCard(deckId, "Hola", "Hello");
	const int dueToday = deckDb.SeedCard(deckId, "Adios", "Goodbye");
	const int notDue = deckDb.SeedCard(deckId, "Gracias", "Thanks");
	Card* todayCard = deckDb.FindStoredCard(deckId, dueToday);
	Card* laterCard = deckDb.FindStoredCard(deckId, notDue);
	REQUIRE(todayCard != nullptr);
	REQUIRE(laterCard != nullptr);
	todayCard->SetNextReviewDate(TestDate());
	laterCard->SetNextReviewDate(TestDate().AddDays(1));

	FakeDateProviderService dates(TestDate());
	StudyDeckUseCase useCase(deckDb, dates);

	StudyDeckRequest request;
	request.deckId = deckId;
	StudyDeckResponse response = useCase.Execute(request);

	CHECK(response.deckId == deckId);
	CHECK(response.deckName == "Spanish");
	REQUIRE(response.dueCards.size() == 2);
	CHECK(response.dueCards[0].cardId == dueNew);
	CHECK(response.dueCards[0].front == "Hola");
	CHECK(response.dueCards[1].cardId == dueToday);
	CHECK(response.dueCards[1].front == "Adios");
}

TEST_CASE("StudyDeckUseCase returns an empty due list when the deck is missing") {
	FakeDeckDbService deckDb;
	FakeDateProviderService dates(TestDate());
	StudyDeckUseCase useCase(deckDb, dates);

	StudyDeckRequest request;
	request.deckId = 99;
	StudyDeckResponse response = useCase.Execute(request);

	CHECK(response.deckId == 99);
	CHECK(response.deckName == "");
	CHECK(response.dueCards.empty());
}
