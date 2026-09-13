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
		int correct = 0;
		int wrong = 0;
		int setCount = 0;
		int clearCount = 0;
		int recordCount = 0;

		void SetSitting(
			int deckId,
			const std::string& deckName,
			const std::vector<AiQuestion>& questions
		) override {
			this->deckId = deckId;
			this->deckName = deckName;
			this->questions = questions;
			this->correct = 0;
			this->wrong = 0;
			this->setCount += 1;
		}

		int GetDeckId() const override { return this->deckId; }
		const std::string& GetDeckName() const override { return this->deckName; }
		const std::vector<AiQuestion>& GetQuestions() const override { return this->questions; }

		bool RecordGrade(bool gotIt) override {
			this->recordCount += 1;
			if (this->questions.empty()) {
				return false;
			}
			if (this->correct + this->wrong >= static_cast<int>(this->questions.size())) {
				return false;
			}
			if (gotIt) {
				this->correct += 1;
			} else {
				this->wrong += 1;
			}
			return true;
		}

		int GetCorrect() const override { return this->correct; }
		int GetWrong() const override { return this->wrong; }

		void Clear() override {
			this->deckId = 0;
			this->deckName.clear();
			this->questions.clear();
			this->correct = 0;
			this->wrong = 0;
			this->clearCount += 1;
		}
};

#endif
