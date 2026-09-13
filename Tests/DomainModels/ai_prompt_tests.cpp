#include "doctest/doctest.h"
#include <string>
#include "../../DomainModels/ai_prompt.h"

TEST_CASE("AiPrompt::Build asks for one tagged question per card") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(1, 1, "Hola", "Hello"));
	deck.AddCard(Card(2, 1, "Adios", "Goodbye"));

	const std::string text = AiPrompt::Build(deck).GetText();

	CHECK(text.find("EACH flashcard") != std::string::npos);
	CHECK(text.find("sentence or fill-in") != std::string::npos);
	CHECK(text.find("Do not put a blank in the sentence") != std::string::npos);
	CHECK(text.find("cloze with ___") != std::string::npos);
	CHECK(text.find("Deck: Spanish") != std::string::npos);
	CHECK(text.find("Front: Hola | Back: Hello") != std::string::npos);
	CHECK(text.find("Front: Adios | Back: Goodbye") != std::string::npos);
	CHECK(text.find("User:") == std::string::npos);
}

TEST_CASE("AiPrompt::Build includes every card in the deck") {
	Deck deck(1, "Spanish");
	deck.AddCard(Card(1, 1, "front-1", "back-1"));
	deck.AddCard(Card(2, 1, "front-2", "back-2"));
	deck.AddCard(Card(3, 1, "front-3", "back-3"));

	const std::string text = AiPrompt::Build(deck).GetText();

	CHECK(text.find("Front: front-1 | Back: back-1") != std::string::npos);
	CHECK(text.find("Front: front-2 | Back: back-2") != std::string::npos);
	CHECK(text.find("Front: front-3 | Back: back-3") != std::string::npos);
}
