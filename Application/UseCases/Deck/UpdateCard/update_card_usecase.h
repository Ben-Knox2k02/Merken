#ifndef UPDATE_CARD_USE_CASE_H
#define UPDATE_CARD_USE_CASE_H

#include "update_card_request.h"
#include "../../../ServiceInterfaces/deck_repository.h"

class UpdateCardUseCase {
	public:
		explicit UpdateCardUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		bool Execute(const UpdateCardRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
