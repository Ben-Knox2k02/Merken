#ifndef CREATE_CARD_USE_CASE_H
#define CREATE_CARD_USE_CASE_H

#include "create_card_request.h"
#include "create_card_response.h"
#include "../../../ServiceInterfaces/deck_repository.h"

class CreateCardUseCase {
	public:
		explicit CreateCardUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		CreateCardResponse Execute(const CreateCardRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
