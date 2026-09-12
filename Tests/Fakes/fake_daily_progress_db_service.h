#ifndef FAKE_DAILY_PROGRESS_DB_SERVICE_H
#define FAKE_DAILY_PROGRESS_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../Application/ServiceInterfaces/daily_progress_db_service.h"

class FakeDailyProgressDbService : public IDailyProgressDBService {
	public:
		std::vector<DailyProgress> rows;
		int addCount = 0;
		int updateCount = 0;

		std::optional<DailyProgress> GetDailyProgress(const Date& date) override {
			for (const DailyProgress& progress : this->rows) {
				if (progress.GetDate() == date) {
					return progress;
				}
			}
			return std::nullopt;
		}

		std::vector<DailyProgress> GetDailyProgressRange(const Date& startDate, const Date& endDate) override {
			std::vector<DailyProgress> matching;
			for (const DailyProgress& progress : this->rows) {
				if (progress.GetDate() >= startDate && progress.GetDate() <= endDate) {
					matching.push_back(progress);
				}
			}
			return matching;
		}

		bool AddDailyProgress(const DailyProgress& progress) override {
			this->rows.push_back(progress);
			this->addCount += 1;
			return true;
		}

		bool UpdateDailyProgress(const DailyProgress&) override {
			this->updateCount += 1;
			return true;
		}
};

#endif
