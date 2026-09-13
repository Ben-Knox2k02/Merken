#ifndef REVIEW_CARD_USE_CASE_H
#define REVIEW_CARD_USE_CASE_H

#include "review_card_request.h"
#include "review_card_response.h"
#include "../../../Repositories/deck_repository.h"
#include "../../../Repositories/daily_progress_repository.h"
#include "../../../Services/date_provider_service.h"

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
