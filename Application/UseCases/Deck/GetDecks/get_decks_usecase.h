#ifndef GET_DECKS_USE_CASE_H
#define GET_DECKS_USE_CASE_H

#include "get_decks_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class GetDecksUseCase {
	public:
		explicit GetDecksUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		GetDecksResponse Execute();

	private:
		IDeckDBService& deckDBService;
};

#endif
