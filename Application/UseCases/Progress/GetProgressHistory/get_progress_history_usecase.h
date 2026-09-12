#ifndef GET_PROGRESS_HISTORY_USE_CASE_H
#define GET_PROGRESS_HISTORY_USE_CASE_H

#include "get_progress_history_request.h"
#include "get_progress_history_response.h"
#include "../../../ServiceInterfaces/daily_progress_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class GetProgressHistoryUseCase {
	public:
		GetProgressHistoryUseCase(
			IDailyProgressDBService& dailyProgressDBService,
			IDateProviderService& dateProviderService
		) : dailyProgressDBService(dailyProgressDBService),
			dateProviderService(dateProviderService) {}

		GetProgressHistoryResponse Execute(const GetProgressHistoryRequest& request);

	private:
		IDailyProgressDBService& dailyProgressDBService;
		IDateProviderService& dateProviderService;
};

#endif
