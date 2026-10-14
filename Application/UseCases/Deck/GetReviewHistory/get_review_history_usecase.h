#ifndef GET_REVIEW_HISTORY_USE_CASE_H
#define GET_REVIEW_HISTORY_USE_CASE_H

#include "get_review_history_request.h"
#include "get_review_history_response.h"
#include "../../../Repositories/deck_repository.h"

class GetReviewHistoryUseCase {
	public:
		explicit GetReviewHistoryUseCase(IDeckRepository& deckRepository)
			: deckRepository(deckRepository) {}

		GetReviewHistoryResponse Execute(const GetReviewHistoryRequest& request);

	private:
		IDeckRepository& deckRepository;
};

#endif
