#ifndef DAILY_PROGRESS_H
#define DAILY_PROGRESS_H

#include "date.h"

class DailyProgress {
	public:
		explicit DailyProgress(const Date& date, int cardsReviewed = 0, int cardsCorrect = 0)
			: date(date), cardsReviewed(cardsReviewed), cardsCorrect(cardsCorrect) {}

		const Date& GetDate() const { return this->date; }
		int GetCardsReviewed() const { return this->cardsReviewed; }
		int GetCardsCorrect() const { return this->cardsCorrect; }

		void RecordCard(bool remembered) {
			this->cardsReviewed += 1;
			if (remembered) {
				this->cardsCorrect += 1;
			}
		}

		double RetentionRate() const {
			if (this->cardsReviewed == 0) {
				return 0.0;
			}
			return static_cast<double>(this->cardsCorrect) / this->cardsReviewed;
		}

	private:
		Date date;
		int cardsReviewed;
		int cardsCorrect;
};

#endif
