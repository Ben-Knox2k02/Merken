#ifndef CREATE_CARD_USE_CASE_H
#define CREATE_CARD_USE_CASE_H

#include "create_card_request.h"
#include "create_card_response.h"
#include "../../../ServiceInterfaces/card_db_service.h"

class CreateCardUseCase {
	public:
		explicit CreateCardUseCase(ICardDBService& cardDBService)
			: cardDBService(cardDBService) {}

		CreateCardResponse Execute(const CreateCardRequest& request);

	private:
		ICardDBService& cardDBService;
};

#endif
