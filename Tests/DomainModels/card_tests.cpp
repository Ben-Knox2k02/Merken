#include "doctest/doctest.h"
#include "../../DomainModels/card.h"

namespace {
Date MustCreate(int year, int month, int day) {
	std::optional<Date> date = Date::Create(year, month, day);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("Card starts as a new recall card") {
	Card card(1, 2, "Hola", "Hello");
	CHECK(card.GetCardId() == 1);
	CHECK(card.GetDeckId() == 2);
	CHECK(card.GetFront() == "Hola");
	CHECK(card.GetBack() == "Hello");
	CHECK(card.GetTags().empty());
	CHECK(card.GetCardType() == CardType::Recall);
	CHECK(card.GetChoices().empty());
	CHECK(card.GetIntervalDays() == 0);
	CHECK(card.GetEaseFactor() == doctest::Approx(2.5));
	CHECK(card.GetRepetitionCount() == 0);
	CHECK_FALSE(card.GetNextReviewDate().has_value());
	CHECK_FALSE(card.GetLastReviewedDate().has_value());
}

TEST_CASE("Card::IsDue treats missing and past next-review dates as due") {
	Card card(1, 1, "Hola", "Hello");
	const Date today = MustCreate(2026, 9, 12);
	CHECK(card.IsDue(today));

	card.SetNextReviewDate(today);
	CHECK(card.IsDue(today));
	CHECK(card.IsDue(today.AddDays(1)));

	card.SetNextReviewDate(today.AddDays(1));
	CHECK_FALSE(card.IsDue(today));
	CHECK(card.IsDue(today.AddDays(1)));
}

TEST_CASE("Card content, tags, and type can be updated") {
	Card card(1, 1, "Hola", "Hello");
	card.UpdateContent("Buenos dias", "Good morning");
	card.UpdateTags({Tag("Greeting"), Tag("Formal")});
	CHECK(card.GetFront() == "Buenos dias");
	CHECK(card.GetBack() == "Good morning");
	REQUIRE(card.GetTags().size() == 2);
	CHECK(card.GetTags()[0].GetName() == "Greeting");

	card.SetMultipleChoice({"A", "B", "C"});
	CHECK(card.GetCardType() == CardType::MultipleChoice);
	REQUIRE(card.GetChoices().size() == 3);
	CHECK(card.GetChoices()[0] == "A");

	card.SetRecall();
	CHECK(card.GetCardType() == CardType::Recall);
	CHECK(card.GetChoices().empty());
}

TEST_CASE("Card::RecordReview applies SM-2 intervals for remembered cards") {
	Card card(1, 1, "Hola", "Hello");
	const Date today = MustCreate(2026, 9, 12);

	card.RecordReview(true, today);
	CHECK(card.GetRepetitionCount() == 1);
	CHECK(card.GetIntervalDays() == 1);
	CHECK(card.GetEaseFactor() == doctest::Approx(2.6));
	REQUIRE(card.GetLastReviewedDate().has_value());
	CHECK(*card.GetLastReviewedDate() == today);

	card.RecordReview(true, today.AddDays(1));
	CHECK(card.GetRepetitionCount() == 2);
	CHECK(card.GetIntervalDays() == 6);
	CHECK(card.GetEaseFactor() == doctest::Approx(2.7));

	card.RecordReview(true, today.AddDays(7));
	CHECK(card.GetRepetitionCount() == 3);
	CHECK(card.GetIntervalDays() == 16);
	CHECK(card.GetEaseFactor() == doctest::Approx(2.8));
}

TEST_CASE("Card::RecordReview resets a forgotten card") {
	Card card(1, 1, "Hola", "Hello");
	const Date today = MustCreate(2026, 9, 12);
	card.SetReviewState(16, 2.8, 3);

	card.RecordReview(false, today);
	CHECK(card.GetRepetitionCount() == 0);
	CHECK(card.GetIntervalDays() == 1);
	CHECK(card.GetEaseFactor() == doctest::Approx(2.6));
	REQUIRE(card.GetLastReviewedDate().has_value());
	CHECK(*card.GetLastReviewedDate() == today);
}

TEST_CASE("Card::RecordReview does not drop ease at or below 1.3") {
	Card card(1, 1, "Hola", "Hello");
	card.SetReviewState(1, 1.3, 0);
	card.RecordReview(false, MustCreate(2026, 9, 12));
	CHECK(card.GetEaseFactor() == doctest::Approx(1.3));
}
