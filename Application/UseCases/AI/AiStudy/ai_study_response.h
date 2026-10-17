#ifndef AI_STUDY_RESPONSE_H
#define AI_STUDY_RESPONSE_H

#include <string>
#include <vector>
#include "ai_question_response.h"

struct AiStudyResponse {
	bool success;
	std::string deckName;
	std::vector<AiQuestionResponse> questions;
};

#endif
