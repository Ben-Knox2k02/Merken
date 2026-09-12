#ifndef GET_CARDS_USE_CASE_H
#define GET_CARDS_USE_CASE_H

#include "get_cards_request.h"
#include "get_cards_response.h"
#include "../../../ServiceInterfaces/deck_db_service.h"

class GetCardsUseCase {
	public:
		explicit GetCardsUseCase(IDeckDBService& deckDBService)
			: deckDBService(deckDBService) {}

		GetCardsResponse Execute(const GetCardsRequest& request);

	private:
		IDeckDBService& deckDBService;
};

#endif
