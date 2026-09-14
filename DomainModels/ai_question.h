#ifndef AI_QUESTION_H
#define AI_QUESTION_H

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "card.h"

enum class AiQuestionType {
	Sentence,
	FillIn
};

class AiQuestion {
	public:
		AiQuestion(
			int cardId,
			AiQuestionType type,
			const std::string& text,
			const std::string& back
		) : cardId(cardId),
			type(type),
			text(text),
			back(back) {}

		int GetCardId() const { return this->cardId; }
		AiQuestionType GetType() const { return this->type; }
		const std::string& GetText() const { return this->text; }
		const std::string& GetBack() const { return this->back; }

		static std::optional<AiQuestion> Parse(const std::string& line, const Card& card) {
			const std::string trimmed = Trim(line);
			if (trimmed.empty()) {
				return std::nullopt;
			}

			AiQuestionType type = AiQuestionType::Sentence;
			std::string text = StripNumberPrefix(trimmed);
			const auto pipe = text.find('|');
			if (pipe != std::string::npos) {
				type = ParseType(text.substr(0, pipe));
				text = Trim(text.substr(pipe + 1));
			} else if (text.find("___") != std::string::npos) {
				type = AiQuestionType::FillIn;
			}
			if (text.empty()) {
				return std::nullopt;
			}
			return AiQuestion(card.GetCardId(), type, text, card.GetFront());
		}

		static std::vector<AiQuestion> ParseMultiple(
			const std::string& reply,
			const std::vector<Card>& cards
		) {
			std::vector<AiQuestion> questions;
			if (cards.empty()) {
				return questions;
			}

			std::stringstream stream(reply);
			std::string line;
			while (std::getline(stream, line) && questions.size() < cards.size()) {
				std::optional<AiQuestion> question = Parse(line, cards[questions.size()]);
				if (question.has_value()) {
					questions.push_back(*question);
				}
			}

			if (questions.empty()) {
				std::optional<AiQuestion> question = Parse(reply, cards.front());
				if (question.has_value()) {
					questions.push_back(*question);
				}
			}
			return questions;
		}

	private:
		static std::string Trim(const std::string& value) {
			const auto start = value.find_first_not_of(" \t\r\n");
			if (start == std::string::npos) {
				return "";
			}
			const auto end = value.find_last_not_of(" \t\r\n");
			return value.substr(start, end - start + 1);
		}

		static std::string StripNumberPrefix(const std::string& value) {
			size_t i = 0;
			if (i < value.size() && std::isdigit(static_cast<unsigned char>(value[i]))) {
				while (i < value.size() && std::isdigit(static_cast<unsigned char>(value[i]))) {
					i += 1;
				}
				if (i < value.size() && (value[i] == '.' || value[i] == ')' || value[i] == ':')) {
					i += 1;
					return Trim(value.substr(i));
				}
			}
			return value;
		}

		static AiQuestionType ParseType(const std::string& token) {
			std::string lower = Trim(token);
			for (char& c : lower) {
				c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			}
			if (lower == "fill-in" || lower == "fillin" || lower == "fill_in") {
				return AiQuestionType::FillIn;
			}
			return AiQuestionType::Sentence;
		}

		int cardId;
		AiQuestionType type;
		std::string text;
		std::string back;
};

#endif
