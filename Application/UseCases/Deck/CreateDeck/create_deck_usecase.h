#ifndef CREATE_DECK_USE_CASE_H
#define CREATE_DECK_USE_CASE_H

#include "create_deck_request.h"
#include "create_deck_response.h"
#include "../../../Repositories/deck_repository.h"
#include "../../../Services/date_provider_service.h"

class CreateDeckUseCase {
	public:
		CreateDeckUseCase(IDeckRepository& deckRepository, IDateProviderService& dateProviderService)
			: deckRepository(deckRepository), dateProviderService(dateProviderService) {}

		CreateDeckResponse Execute(const CreateDeckRequest& request);

	private:
		IDeckRepository& deckRepository;
		IDateProviderService& dateProviderService;
};

#endif
