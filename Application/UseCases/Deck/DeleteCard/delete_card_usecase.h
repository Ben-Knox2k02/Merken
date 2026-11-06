#ifndef DELETE_CARD_USE_CASE_H
#define DELETE_CARD_USE_CASE_H

#include "delete_card_request.h"
#include "../../../Repositories/deck_repository.h"

class DeleteCardUseCase {
	public:
		explicit DeleteCardUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		bool Execute(const DeleteCardRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
