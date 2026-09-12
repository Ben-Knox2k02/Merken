#include "doctest/doctest.h"
#include "../../DomainModels/deck.h"

namespace {
Date MustCreate(int year, int month, int day) {
	std::optional<Date> date = Date::Create(year, month, day);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("Deck stores identity and optional description") {
	Deck deck(7, "Spanish", "Vocabulary", "2026-09-12");
	CHECK(deck.GetDeckId() == 7);
	CHECK(deck.GetName() == "Spanish");
	CHECK(deck.GetDescription() == "Vocabulary");
	CHECK(deck.GetCreatedAt() == "2026-09-12");
	CHECK(deck.GetCards().empty());

	Deck unnamed(1, "History");
	CHECK(unnamed.GetDescription() == "");
	CHECK(unnamed.GetCreatedAt() == "");
}

TEST_CASE("Deck can update name and description") {
	Deck deck(1, "Old", "Old desc");
	deck.UpdateName("New");
	deck.UpdateDescription("New desc");
	CHECK(deck.GetName() == "New");
	CHECK(deck.GetDescription() == "New desc");
}

TEST_CASE("Deck add, find, and remove cards") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(10, 1, "Hola", "Hello"));
	deck.AddCard(Card(11, 1, "Adios", "Goodbye"));
	REQUIRE(deck.GetCards().size() == 2);

	Card* found = deck.FindCard(11);
	REQUIRE(found != nullptr);
	CHECK(found->GetFront() == "Adios");
	CHECK(deck.FindCard(99) == nullptr);

	deck.RemoveCard(10);
	REQUIRE(deck.GetCards().size() == 1);
	CHECK(deck.GetCards()[0].GetCardId() == 11);

	deck.RemoveCard(99);
	CHECK(deck.GetCards().size() == 1);
}

TEST_CASE("Deck::GetDueCards returns only cards due on the given date") {
	Deck deck(1, "Spanish");
	const Date today = MustCreate(2026, 9, 12);
	Card dueNew(1, 1, "Hola", "Hello");
	Card dueToday(2, 1, "Adios", "Goodbye");
	dueToday.SetNextReviewDate(today);
	Card later(3, 1, "Gracias", "Thanks");
	later.SetNextReviewDate(today.AddDays(1));
	deck.AddCard(dueNew);
	deck.AddCard(dueToday);
	deck.AddCard(later);

	std::vector<Card> due = deck.GetDueCards(today);
	REQUIRE(due.size() == 2);
	CHECK(due[0].GetCardId() == 1);
	CHECK(due[1].GetCardId() == 2);
}
