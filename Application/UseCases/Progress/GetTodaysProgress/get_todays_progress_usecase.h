#ifndef GET_TODAYS_PROGRESS_USE_CASE_H
#define GET_TODAYS_PROGRESS_USE_CASE_H

#include "get_todays_progress_response.h"
#include "../../../ServiceInterfaces/daily_progress_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class GetTodaysProgressUseCase {
	public:
		GetTodaysProgressUseCase(
			IDailyProgressDBService& dailyProgressDBService,
			IDateProviderService& dateProviderService
		) : dailyProgressDBService(dailyProgressDBService),
			dateProviderService(dateProviderService) {}

		GetTodaysProgressResponse Execute();

	private:
		IDailyProgressDBService& dailyProgressDBService;
		IDateProviderService& dateProviderService;
};

#endif
