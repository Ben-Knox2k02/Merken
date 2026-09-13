#ifndef AI_PROMPT_H
#define AI_PROMPT_H

#include <string>
#include "deck.h"

class AiPrompt {
	public:
		static AiPrompt Build(const Deck& deck) {
			std::string text =
				"Write one question for EACH flashcard below, in the same order. "
				"Each line must be: <n>. <type> | <question> "
				"where type is sentence or fill-in. "
				"sentence: a question in words ending with ? Do not put a blank in the sentence. "
				"fill-in: a cloze with ___ where the answer goes. Do not include the answer. "
				"Use only the facts on the cards. Do not invent facts that are not on the cards.\n\n"
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
