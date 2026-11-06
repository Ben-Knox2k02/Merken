#ifndef DAILY_PROGRESS_REPOSITORY_REAL_H
#define DAILY_PROGRESS_REPOSITORY_REAL_H

#include "../../../Application/Repositories/daily_progress_repository.h"
#include "../DatabaseContext/database_context.h"

class DailyProgressRepository : public IDailyProgressRepository {
	public:
		DailyProgressRepository(DatabaseContext& dbContext) : db(dbContext) {}

		std::optional<DailyProgress> GetDailyProgress(const Date& date) override;
		std::vector<DailyProgress> GetDailyProgressRange(const Date& startDate, const Date& endDate) override;
		bool AddDailyProgress(const DailyProgress& progress) override;
		bool UpdateDailyProgress(const DailyProgress& progress) override;

	private:
		DatabaseContext& db;
};

#endif
