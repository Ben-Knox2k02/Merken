#ifndef DAILY_PROGRESS_H
#define DAILY_PROGRESS_H

#include <string>

class DailyProgress {
	public:
		explicit DailyProgress(const std::string& date)
			: date(date), cardsReviewed(0), cardsCorrect(0) {}

		const std::string& GetDate() const { return this->date; }
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
		std::string date;
		int cardsReviewed;
		int cardsCorrect;
};

#endif
