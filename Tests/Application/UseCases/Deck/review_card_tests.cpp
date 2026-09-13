#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Deck/ReviewCard/review_card_usecase.h"
#include "../../../Fakes/fake_deck_repository.h"
#include "../../../Fakes/fake_daily_progress_repository.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 12);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("ReviewCardUseCase records a remembered review") {
	FakeDeckRepository deckRepository;
	FakeDailyProgressRepository progressRepository;
	FakeDateProviderService dates(TestDate());
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	ReviewCardUseCase useCase(deckRepository, progressRepository, dates);

	ReviewCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.remembered = true;

	ReviewCardResponse response = useCase.Execute(request);

	CHECK(response.success);
	CHECK(response.cardId == cardId);
	CHECK(response.intervalDays == 1);
	CHECK(response.nextReviewDate == "2026-09-13");
	CHECK(response.cardsReviewed == 1);
	CHECK(response.cardsCorrect == 1);
	CHECK(response.retentionRate == doctest::Approx(1.0));
	CHECK(progressRepository.addCount == 1);
	CHECK(progressRepository.updateCount == 0);

	Card* stored = deckRepository.FindStoredCard(deckId, cardId);
	REQUIRE(stored != nullptr);
	CHECK(stored->GetIntervalDays() == 1);
	CHECK(stored->GetRepetitionCount() == 1);
	REQUIRE(stored->GetNextReviewDate().has_value());
	CHECK(stored->GetNextReviewDate()->ToIso() == "2026-09-13");
}

TEST_CASE("ReviewCardUseCase records a forgotten review and updates existing progress") {
	FakeDeckRepository deckRepository;
	FakeDailyProgressRepository progressRepository;
	FakeDateProviderService dates(TestDate());
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int firstId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	const int secondId = deckRepository.SeedCard(deckId, "Adios", "Goodbye");
	ReviewCardUseCase useCase(deckRepository, progressRepository, dates);

	ReviewCardRequest remembered;
	remembered.deckId = deckId;
	remembered.cardId = firstId;
	remembered.remembered = true;
	CHECK(useCase.Execute(remembered).success);

	ReviewCardRequest forgot;
	forgot.deckId = deckId;
	forgot.cardId = secondId;
	forgot.remembered = false;
	ReviewCardResponse response = useCase.Execute(forgot);

	CHECK(response.success);
	CHECK(response.intervalDays == 1);
	CHECK(response.nextReviewDate == "2026-09-13");
	CHECK(response.cardsReviewed == 2);
	CHECK(response.cardsCorrect == 1);
	CHECK(response.retentionRate == doctest::Approx(0.5));
	CHECK(progressRepository.addCount == 1);
	CHECK(progressRepository.updateCount == 1);

	Card* stored = deckRepository.FindStoredCard(deckId, secondId);
	REQUIRE(stored != nullptr);
	CHECK(stored->GetRepetitionCount() == 0);
	CHECK(stored->GetEaseFactor() == doctest::Approx(2.3));
}

TEST_CASE("ReviewCardUseCase fails when the deck or card is missing") {
	FakeDeckRepository deckRepository;
	FakeDailyProgressRepository progressRepository;
	FakeDateProviderService dates(TestDate());
	const int deckId = deckRepository.SeedDeck("Spanish");
	ReviewCardUseCase useCase(deckRepository, progressRepository, dates);

	ReviewCardRequest missingDeck;
	missingDeck.deckId = 99;
	missingDeck.cardId = 1;
	missingDeck.remembered = true;
	CHECK_FALSE(useCase.Execute(missingDeck).success);

	ReviewCardRequest missingCard;
	missingCard.deckId = deckId;
	missingCard.cardId = 99;
	missingCard.remembered = true;
	CHECK_FALSE(useCase.Execute(missingCard).success);
	CHECK(progressRepository.addCount == 0);
}

TEST_CASE("ReviewCardUseCase fails when the card update is not persisted") {
	FakeDeckRepository deckRepository;
	FakeDailyProgressRepository progressRepository;
	FakeDateProviderService dates(TestDate());
	const int deckId = deckRepository.SeedDeck("Spanish");
	const int cardId = deckRepository.SeedCard(deckId, "Hola", "Hello");
	deckRepository.updateCardSucceeds = false;
	ReviewCardUseCase useCase(deckRepository, progressRepository, dates);

	ReviewCardRequest request;
	request.deckId = deckId;
	request.cardId = cardId;
	request.remembered = true;

	CHECK_FALSE(useCase.Execute(request).success);
	CHECK(progressRepository.addCount == 0);
}
