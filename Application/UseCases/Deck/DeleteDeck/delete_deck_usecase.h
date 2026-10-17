#ifndef DELETE_DECK_USE_CASE_H
#define DELETE_DECK_USE_CASE_H

#include "delete_deck_request.h"
#include "../../../Repositories/deck_repository.h"

class DeleteDeckUseCase {
	public:
		explicit DeleteDeckUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		bool Execute(const DeleteDeckRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
