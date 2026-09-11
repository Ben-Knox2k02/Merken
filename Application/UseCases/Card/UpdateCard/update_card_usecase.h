#ifndef UPDATE_CARD_USE_CASE_H
#define UPDATE_CARD_USE_CASE_H

#include "update_card_request.h"
#include "../../../ServiceInterfaces/card_db_service.h"

class UpdateCardUseCase {
	public:
		explicit UpdateCardUseCase(ICardDBService& cardDBService)
			: cardDBService(cardDBService) {}

		bool Execute(const UpdateCardRequest& request);

	private:
		ICardDBService& cardDBService;
};

#endif
