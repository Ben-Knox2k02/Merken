#ifndef CREATE_DECK_USE_CASE_H
#define CREATE_DECK_USE_CASE_H

#include "create_deck_request.h"
#include "create_deck_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"
#include "../../../ServiceInterfaces/date_provider_service.h"

class CreateDeckUseCase {
	public:
		CreateDeckUseCase(IDeckDBService& deckDBService, IDateProviderService& dateProviderService)
			: deckDBService(deckDBService), dateProviderService(dateProviderService) {}

		CreateDeckResponse Execute(const CreateDeckRequest& request);

	private:
		IDeckDBService& deckDBService;
		IDateProviderService& dateProviderService;
};

#endif
