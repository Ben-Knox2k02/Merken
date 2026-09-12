#ifndef CREATE_CARD_USE_CASE_H
#define CREATE_CARD_USE_CASE_H

#include "create_card_request.h"
#include "create_card_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class CreateCardUseCase {
	public:
		explicit CreateCardUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		CreateCardResponse Execute(const CreateCardRequest& request);

	private:
		IDeckDBService& deckDBService;
};

#endif
