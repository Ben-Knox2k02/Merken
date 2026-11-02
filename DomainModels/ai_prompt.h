#ifndef AI_PROMPT_H
#define AI_PROMPT_H

#include <string>
#include "deck.h"

class AiPrompt {
	public:
		static AiPrompt Build(const Deck& deck) {
			std::string text =
				"Write one question for EACH flashcard below, in the same order. "
				"Treat each card as a topic, not a script." 
				"Ask a question that checks whether the student understands the idea: why it is true, how to use it, or how it differs from something similar."
				"Do not just restate the front of the card. A correct answer must be supportable by what is on that card."
				"Do not invent facts that are not on the cards.\n\n"
				"Each line must be: <n>. <type> | <question> | <answer> where type is sentence or fill-in and answer is the short correct answer"
				"to YOUR question (for fill-in, the exact words that go in the blank)."
				"The answer answer must be supportable by the card."
				"Do not use the | character inside the question or the answer."
				"Reply with only the numbered lines."
				"Deck: " + deck.GetName() + "\n"
				"Cards:\n";

			for (const Card& card : deck.GetCards()) {
				text += "- Front: " + card.GetFront() + " | Back: " + card.GetBack() + "\n";
			}
			return AiPrompt(text);
		}

		const std::string& GetText() const { return this->text; }

	private:
		explicit AiPrompt(const std::string& text) : text(text) {}

		std::string text;
};

#endif
