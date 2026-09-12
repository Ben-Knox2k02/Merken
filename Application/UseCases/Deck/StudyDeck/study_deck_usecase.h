#ifndef STUDY_DECK_USE_CASE_H
#define STUDY_DECK_USE_CASE_H

#include "study_deck_request.h"
#include "study_deck_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class StudyDeckUseCase {
	public:
		StudyDeckUseCase(
			IDeckDBService& deckDBService,
			IDateProviderService& dateProviderService
		) : deckDBService(deckDBService),
			dateProviderService(dateProviderService) {}

		StudyDeckResponse Execute(const StudyDeckRequest& request);

	private:
		IDeckDBService& deckDBService;
		IDateProviderService& dateProviderService;
};

#endif
