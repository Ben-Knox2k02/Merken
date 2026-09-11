#ifndef CREATE_DECK_USE_CASE_H
#define CREATE_DECK_USE_CASE_H

#include "create_deck_request.h"
#include "create_deck_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class CreateDeckUseCase {
	public:
		explicit CreateDeckUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		CreateDeckResponse Execute(const CreateDeckRequest& request);

	private:
		IDeckDBService& deckDBService;
};

#endif
