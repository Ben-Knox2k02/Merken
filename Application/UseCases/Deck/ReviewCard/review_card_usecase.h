#ifndef REVIEW_CARD_USE_CASE_H
#define REVIEW_CARD_USE_CASE_H

#include "review_card_request.h"
#include "review_card_response.h"
#include "../../../ServiceInterfaces/deck_repository.h"
#include "../../../ServiceInterfaces/daily_progress_repository.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class ReviewCardUseCase {
	public:
		ReviewCardUseCase(
			IDeckRepository& deckRepository,
			IDailyProgressRepository& dailyProgressRepository,
			IDateProviderService& dateProviderService
		) : deckRepository(deckRepository),
			dailyProgressRepository(dailyProgressRepository),
			dateProviderService(dateProviderService) {}

		ReviewCardResponse Execute(const ReviewCardRequest& request);

	private:
		IDeckRepository& deckRepository;
		IDailyProgressRepository& dailyProgressRepository;
		IDateProviderService& dateProviderService;
};

#endif
