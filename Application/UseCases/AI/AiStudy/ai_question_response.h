#ifndef AI_QUESTION_RESPONSE_H
#define AI_QUESTION_RESPONSE_H

#include <string>

enum class AiQuestionTypeResponse {
	Sentence,
	FillIn
};

struct AiQuestionResponse {
	int cardId;
	AiQuestionTypeResponse type;
	std::string text;
	std::string back;
};

#endif
