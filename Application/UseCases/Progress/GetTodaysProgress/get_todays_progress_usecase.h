#ifndef GET_TODAYS_PROGRESS_USE_CASE_H
#define GET_TODAYS_PROGRESS_USE_CASE_H

#include "get_todays_progress_response.h"
#include "../../../Repositories/daily_progress_repository.h"
#include "../../../Services/date_provider_service.h"

class GetTodaysProgressUseCase {
	public:
		GetTodaysProgressUseCase(
			IDailyProgressRepository& dailyProgressRepository,
			IDateProviderService& dateProviderService
		) : dailyProgressRepository(dailyProgressRepository),
			dateProviderService(dateProviderService) {}

		GetTodaysProgressResponse Execute();

	private:
		IDailyProgressRepository& dailyProgressRepository;
		IDateProviderService& dateProviderService;
};

#endif
