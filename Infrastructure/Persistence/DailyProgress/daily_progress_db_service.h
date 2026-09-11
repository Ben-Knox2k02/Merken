#ifndef DAILY_PROGRESS_DB_SERVICE_REAL_H
#define DAILY_PROGRESS_DB_SERVICE_REAL_H

#include "../../../Application/ServiceInterfaces/daily_progress_db_service.h"
#include "../DatabaseContext/database_context.h"

class DailyProgressDbService : public IDailyProgressDBService {
	public:
		DailyProgressDbService(DatabaseContext& dbContext) : db(dbContext) {}

		std::optional<DailyProgress> GetDailyProgress(const Date& date) override;
		std::vector<DailyProgress> GetDailyProgressRange(const Date& startDate, const Date& endDate) override;
		bool AddDailyProgress(const DailyProgress& progress) override;
		bool UpdateDailyProgress(const DailyProgress& progress) override;

	private:
		DatabaseContext& db;
};

#endif
