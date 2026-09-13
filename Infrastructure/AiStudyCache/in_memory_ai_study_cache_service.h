#ifndef IN_MEMORY_AI_STUDY_CACHE_SERVICE_H
#define IN_MEMORY_AI_STUDY_CACHE_SERVICE_H

#include <string>
#include <vector>
#include "../../Application/Services/ai_study_cache_service.h"

class InMemoryAiStudyCacheService : public IAiStudyCacheService {
	public:
		void SetSitting(
			int deckId,
			const std::string& deckName,
			const std::vector<AiQuestion>& questions
		) override {
			this->deckId = deckId;
			this->deckName = deckName;
			this->questions = questions;
		}

		int GetDeckId() const override { return this->deckId; }
		const std::string& GetDeckName() const override { return this->deckName; }
		const std::vector<AiQuestion>& GetQuestions() const override { return this->questions; }

		void Clear() override {
			this->deckId = 0;
			this->deckName.clear();
			this->questions.clear();
		}

	private:
		int deckId = 0;
		std::string deckName;
		std::vector<AiQuestion> questions;
};

#endif
