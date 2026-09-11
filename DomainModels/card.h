#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>

enum class CardType {
	Recall,
	MultipleChoice
};

class Card {
	public:
		Card(
			int cardId,
			int deckId,
			const std::string& front,
			const std::string& back
		) : cardId(cardId),
			deckId(deckId),
			front(front),
			back(back),
			cardType(CardType::Recall),
			intervalDays(0),
			easeFactor(2.5),
			repetitionCount(0) {}

		int GetCardId() const { return this->cardId; }
		int GetDeckId() const { return this->deckId; }
		const std::string& GetFront() const { return this->front; }
		const std::string& GetBack() const { return this->back; }
		const std::vector<std::string>& GetTags() const { return this->tags; }
		CardType GetCardType() const { return this->cardType; }
		const std::vector<std::string>& GetChoices() const { return this->choices; }
		int GetIntervalDays() const { return this->intervalDays; }
		double GetEaseFactor() const { return this->easeFactor; }
		int GetRepetitionCount() const { return this->repetitionCount; }
		const std::string& GetNextReviewDate() const { return this->nextReviewDate; }
		const std::string& GetLastReviewedDate() const { return this->lastReviewedDate; }

		void UpdateContent(const std::string& newFront, const std::string& newBack) {
			this->front = newFront;
			this->back = newBack;
		}

		void UpdateTags(const std::vector<std::string>& newTags) {
			this->tags = newTags;
		}

		void SetMultipleChoice(const std::vector<std::string>& newChoices) {
			this->cardType = CardType::MultipleChoice;
			this->choices = newChoices;
		}

		void SetRecall() {
			this->cardType = CardType::Recall;
			this->choices.clear();
		}

		void RecordReview(bool remembered, const std::string& reviewedOnDate) {
			this->lastReviewedDate = reviewedOnDate;

			if (!remembered) {
				this->repetitionCount = 0;
				this->intervalDays = 1;
				if (this->easeFactor > 1.3) {
					this->easeFactor -= 0.2;
				}
			} else {
				this->repetitionCount += 1;
				if (this->repetitionCount == 1) {
					this->intervalDays = 1;
				} else if (this->repetitionCount == 2) {
					this->intervalDays = 6;
				} else {
					this->intervalDays = static_cast<int>(this->intervalDays * this->easeFactor);
				}
				this->easeFactor += 0.1;
			}
		}

		void SetNextReviewDate(const std::string& date) {
			this->nextReviewDate = date;
		}

	private:
		int cardId;
		int deckId;
		std::string front;
		std::string back;
		std::vector<std::string> tags;
		CardType cardType;
		std::vector<std::string> choices;
		int intervalDays;
		double easeFactor;
		int repetitionCount;
		std::string nextReviewDate;
		std::string lastReviewedDate;
};

#endif
