#ifndef GET_DECKS_USE_CASE_H
#define GET_DECKS_USE_CASE_H

#include "get_decks_response.h"
#include "../../../Repositories/deck_repository.h"

class GetDecksUseCase {
	public:
		explicit GetDecksUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		GetDecksResponse Execute();

	private:
		IDeckRepository& deckRepository;
};

#endif
