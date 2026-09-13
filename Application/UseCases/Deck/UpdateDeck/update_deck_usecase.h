#ifndef UPDATE_DECK_USE_CASE_H
#define UPDATE_DECK_USE_CASE_H

#include "update_deck_request.h"
#include "../../../ServiceInterfaces/deck_repository.h"

class UpdateDeckUseCase {
	public:
		explicit UpdateDeckUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		bool Execute(const UpdateDeckRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
