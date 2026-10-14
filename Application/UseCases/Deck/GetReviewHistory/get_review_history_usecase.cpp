#include "get_review_history_usecase.h"

GetReviewHistoryResponse GetReviewHistoryUseCase::Execute(const GetReviewHistoryRequest& request) {
	return GetReviewHistoryResponse{
		.events = this->deckRepository.GetReviewHistory(
			request.startDate,
			request.endDate,
			request.deckId,
			request.cardId
		)
	};
}
