#ifndef STUDY_DECK_USE_CASE_H
#define STUDY_DECK_USE_CASE_H

#include "study_deck_request.h"
#include "study_deck_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"
#include "../../../ServiceInterfaces/card_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class StudyDeckUseCase {
	public:
		StudyDeckUseCase(
			IDeckDBService& deckDBService,
			ICardDBService& cardDBService,
			IDateProviderService& dateProviderService
		) : deckDBService(deckDBService),
			cardDBService(cardDBService),
			dateProviderService(dateProviderService) {}

		StudyDeckResponse Execute(const StudyDeckRequest& request);

	private:
		IDeckDBService& deckDBService;
		ICardDBService& cardDBService;
		IDateProviderService& dateProviderService;
};

#endif
