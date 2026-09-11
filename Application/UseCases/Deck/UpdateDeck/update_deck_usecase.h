#ifndef UPDATE_DECK_USE_CASE_H
#define UPDATE_DECK_USE_CASE_H

#include "update_deck_request.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class UpdateDeckUseCase {
	public:
		explicit UpdateDeckUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		bool Execute(const UpdateDeckRequest& request);

	private:
		IDeckDBService& deckDBService;
};

#endif
