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
	deck.AddCard(Card(1, 1, "Hola", "Hello"));
	deck.AddCard(Card(2, 1, "Adios", "Goodbye"));
	deck.AddCard(Card(3, 1, "Gracias", "Thanks"));

	const std::string text = AiPrompt::Build(deck).GetText();

	CHECK(text.find("Front: Hola | Back: Hello") != std::string::npos);
	CHECK(text.find("Front: Adios | Back: Goodbye") != std::string::npos);
	CHECK(text.find("Front: Gracias | Back: Thanks") != std::string::npos);
}
