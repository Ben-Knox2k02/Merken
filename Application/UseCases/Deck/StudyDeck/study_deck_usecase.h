#ifndef STUDY_DECK_USE_CASE_H
#define STUDY_DECK_USE_CASE_H

#include "study_deck_request.h"
#include "study_deck_response.h"
#include "../../../Repositories/deck_repository.h"
#include "../../../Services/date_provider_service.h"

class StudyDeckUseCase {
	public:
		StudyDeckUseCase(
			IDeckRepository& deckRepository,
			IDateProviderService& dateProviderService
		) : deckRepository(deckRepository),
			dateProviderService(dateProviderService) {}

		StudyDeckResponse Execute(const StudyDeckRequest& request);

	private:
		IDeckRepository& deckRepository;
		IDateProviderService& dateProviderService;
};

#endif
