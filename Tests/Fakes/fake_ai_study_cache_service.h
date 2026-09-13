#ifndef FAKE_AI_STUDY_CACHE_SERVICE_H
#define FAKE_AI_STUDY_CACHE_SERVICE_H

#include <string>
#include <vector>
#include "../../Application/Services/ai_study_cache_service.h"

class FakeAiStudyCacheService : public IAiStudyCacheService {
	public:
		int deckId = 0;
		std::string deckName;
		std::vector<AiQuestion> questions;
		int setCount = 0;
		int clearCount = 0;

		void SetSitting(
			int deckId,
			const std::string& deckName,
			const std::vector<AiQuestion>& questions
		) override {
			this->deckId = deckId;
			this->deckName = deckName;
			this->questions = questions;
			this->setCount += 1;
		}

		int GetDeckId() const override { return this->deckId; }
		const std::string& GetDeckName() const override { return this->deckName; }
		const std::vector<AiQuestion>& GetQuestions() const override { return this->questions; }

		void Clear() override {
			this->deckId = 0;
			this->deckName.clear();
			this->questions.clear();
			this->clearCount += 1;
		}
};

#endif
