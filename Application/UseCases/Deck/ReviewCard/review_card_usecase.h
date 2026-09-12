#ifndef REVIEW_CARD_USE_CASE_H
#define REVIEW_CARD_USE_CASE_H

#include "review_card_request.h"
#include "review_card_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"
#include "../../../ServiceInterfaces/daily_progress_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class ReviewCardUseCase {
	public:
		ReviewCardUseCase(
			IDeckDBService& deckDBService,
			IDailyProgressDBService& dailyProgressDBService,
			IDateProviderService& dateProviderService
		) : deckDBService(deckDBService),
			dailyProgressDBService(dailyProgressDBService),
			dateProviderService(dateProviderService) {}

		ReviewCardResponse Execute(const ReviewCardRequest& request);

	private:
		IDeckDBService& deckDBService;
		IDailyProgressDBService& dailyProgressDBService;
		IDateProviderService& dateProviderService;
};

#endif
