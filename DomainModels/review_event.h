#ifndef REVIEW_EVENT_H
#define REVIEW_EVENT_H

#include "date.h"
#include <optional>

class ReviewEvent {
	public:
		ReviewEvent(
			int reviewId,
			int deckId,
			int cardId,
			const Date& reviewedOn,
			bool remembered,
			int intervalDays,
			double easeFactor,
			int repetitionCount,
			const std::optional<Date>& nextReviewDate
		) : reviewId(reviewId),
			deckId(deckId),
			cardId(cardId),
			reviewedOn(reviewedOn),
			remembered(remembered),
			intervalDays(intervalDays),
			easeFactor(easeFactor),
			repetitionCount(repetitionCount),
			nextReviewDate(nextReviewDate) {}

		int GetReviewId() const { return this->reviewId; }
		int GetDeckId() const { return this->deckId; }
		int GetCardId() const { return this->cardId; }
		const Date& GetReviewedOn() const { return this->reviewedOn; }
		bool WasRemembered() const { return this->remembered; }
		int GetIntervalDays() const { return this->intervalDays; }
		double GetEaseFactor() const { return this->easeFactor; }
		int GetRepetitionCount() const { return this->repetitionCount; }
		const std::optional<Date>& GetNextReviewDate() const { return this->nextReviewDate; }

	private:
		int reviewId;
		int deckId;
		int cardId;
		Date reviewedOn;
		bool remembered;
		int intervalDays;
		double easeFactor;
		int repetitionCount;
		std::optional<Date> nextReviewDate;
};

#endif
