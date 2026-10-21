#ifndef DAILY_PROGRESS_REPOSITORY_H
#define DAILY_PROGRESS_REPOSITORY_H

#include <vector>
#include <optional>
#include "../../DomainModels/daily_progress.h"

class IDailyProgressRepository {
	public:
		virtual ~IDailyProgressRepository() = default;

		virtual std::optional<DailyProgress> GetDailyProgress(const Date& date) = 0;
		virtual std::vector<DailyProgress> GetDailyProgressRange(const Date& startDate, const Date& endDate) = 0;
		virtual bool AddDailyProgress(const DailyProgress& progress) = 0;
		virtual bool UpdateDailyProgress(const DailyProgress& progress) = 0;
};

#endif
