#include "doctest/doctest.h"
#include <optional>
#include "../../DomainModels/ai_question.h"
#include "../../DomainModels/deck.h"

TEST_CASE("AiQuestion stores sentence and fill-in items") {
	const AiQuestion sentence(3, AiQuestionType::Sentence, "What Spanish word means hello?", "Hola");
	CHECK(sentence.GetCardId() == 3);
	CHECK(sentence.GetType() == AiQuestionType::Sentence);
	CHECK(sentence.GetText() == "What Spanish word means hello?");
	CHECK(sentence.GetBack() == "Hola");

	const AiQuestion fillIn(4, AiQuestionType::FillIn, "___ means goodbye.", "Adios");
	CHECK(fillIn.GetType() == AiQuestionType::FillIn);
	CHECK(fillIn.GetText() == "___ means goodbye.");
	CHECK(fillIn.GetBack() == "Adios");
}

TEST_CASE("AiQuestion::Parse reads one tagged line") {
	const Card card(7, 1, "Hola", "Hello");

	std::optional<AiQuestion> fillIn = AiQuestion::Parse("1. fill-in | ___ means hello.", card);
	REQUIRE(fillIn.has_value());
	CHECK(fillIn->GetCardId() == 7);
	CHECK(fillIn->GetType() == AiQuestionType::FillIn);
	CHECK(fillIn->GetText() == "___ means hello.");
	CHECK(fillIn->GetBack() == "Hola");

	std::optional<AiQuestion> sentence = AiQuestion::Parse(
		"2. sentence | What Spanish word means hello?",
		card);
	REQUIRE(sentence.has_value());
	CHECK(sentence->GetType() == AiQuestionType::Sentence);
	CHECK(sentence->GetText() == "What Spanish word means hello?");
	CHECK(sentence->GetBack() == "Hola");
}

TEST_CASE("AiQuestion::Parse treats ___ without a type tag as fill-in") {
	const Card card(7, 1, "Hola", "Hello");

	std::optional<AiQuestion> question = AiQuestion::Parse("1. ___ means hello.", card);
	REQUIRE(question.has_value());
	CHECK(question->GetType() == AiQuestionType::FillIn);
	CHECK(question->GetText() == "___ means hello.");
	CHECK(question->GetBack() == "Hola");
}

TEST_CASE("AiQuestion::Parse returns none for a blank line") {
	const Card card(7, 1, "Hola", "Hello");
	CHECK_FALSE(AiQuestion::Parse("", card).has_value());
	CHECK_FALSE(AiQuestion::Parse("   \t  ", card).has_value());
}

TEST_CASE("AiQuestion::ParseMultiple reads typed lines in card order") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(7, 1, "Hola", "Hello"));
	deck.AddCard(Card(8, 1, "Adios", "Goodbye"));

	const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(
		"1. fill-in | ___ means hello.\n"
		"2. sentence | What Spanish word means goodbye?\n",
		deck.GetCards());

	REQUIRE(questions.size() == 2);
	CHECK(questions[0].GetCardId() == 7);
	CHECK(questions[0].GetType() == AiQuestionType::FillIn);
	CHECK(questions[0].GetText() == "___ means hello.");
	CHECK(questions[0].GetBack() == "Hola");
	CHECK(questions[1].GetCardId() == 8);
	CHECK(questions[1].GetType() == AiQuestionType::Sentence);
	CHECK(questions[1].GetText() == "What Spanish word means goodbye?");
	CHECK(questions[1].GetBack() == "Adios");
}

TEST_CASE("AiQuestion::ParseMultiple ignores extra lines past the card count") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(7, 1, "Hola", "Hello"));

	const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(
		"1. sentence | What Spanish word means hello?\n"
		"2. fill-in | ___ means extra.\n",
		deck.GetCards());

	REQUIRE(questions.size() == 1);
	CHECK(questions[0].GetCardId() == 7);
	CHECK(questions[0].GetText() == "What Spanish word means hello?");
	CHECK(questions[0].GetBack() == "Hola");
}

TEST_CASE("AiQuestion::ParseMultiple treats an unnumbered reply as one sentence") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(7, 1, "Hola", "Hello"));

	const std::vector<AiQuestion> questions = AiQuestion::ParseMultiple(
		"  What Spanish word means hello?  ",
		deck.GetCards());

	REQUIRE(questions.size() == 1);
	CHECK(questions[0].GetType() == AiQuestionType::Sentence);
	CHECK(questions[0].GetText() == "What Spanish word means hello?");
	CHECK(questions[0].GetCardId() == 7);
	CHECK(questions[0].GetBack() == "Hola");
}

TEST_CASE("AiQuestion::ParseMultiple returns none for an empty reply") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(7, 1, "Hola", "Hello"));

	CHECK(AiQuestion::ParseMultiple("", deck.GetCards()).empty());
	CHECK(AiQuestion::ParseMultiple("   \n\t  ", deck.GetCards()).empty());
}
