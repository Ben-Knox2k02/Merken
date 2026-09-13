#ifndef GET_CARDS_USE_CASE_H
#define GET_CARDS_USE_CASE_H

#include "get_cards_request.h"
#include "get_cards_response.h"
#include "../../../ServiceInterfaces/deck_repository.h"

class GetCardsUseCase {
	public:
		explicit GetCardsUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		GetCardsResponse Execute(const GetCardsRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
