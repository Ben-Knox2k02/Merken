#ifndef AI_STUDY_CACHE_SERVICE_H
#define AI_STUDY_CACHE_SERVICE_H

#include <string>
#include <vector>
#include "../../DomainModels/ai_question.h"

class IAiStudyCacheService {
	public:
		virtual ~IAiStudyCacheService() = default;

		virtual void SetSitting(
			int deckId,
			const std::string& deckName,
			const std::vector<AiQuestion>& questions
		) = 0;
		virtual int GetDeckId() const = 0;
		virtual const std::string& GetDeckName() const = 0;
		virtual const std::vector<AiQuestion>& GetQuestions() const = 0;
		virtual void Clear() = 0;
};

#endif
