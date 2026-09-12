#ifndef UPDATE_CARD_USE_CASE_H
#define UPDATE_CARD_USE_CASE_H

#include "update_card_request.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class UpdateCardUseCase {
	public:
		explicit UpdateCardUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		bool Execute(const UpdateCardRequest& request);

	private:
		IDeckDBService& deckDBService;
};

#endif
