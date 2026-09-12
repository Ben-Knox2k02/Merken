#include "review_card_usecase.h"
#include <optional>

ReviewCardResponse ReviewCardUseCase::Execute(const ReviewCardRequest& request) {
	ReviewCardResponse response;
	response.success = false;
	response.cardId = request.cardId;
	response.intervalDays = 0;
	response.cardsReviewed = 0;
	response.cardsCorrect = 0;
	response.retentionRate = 0.0;

	std::optional<Deck> deck = this->deckDBService.GetDeck(request.deckId);
	if (!deck.has_value()) {
		return response;
	}

	Card* card = deck->FindCard(request.cardId);
	if (card == nullptr) {
		return response;
	}

	const Date today = this->dateProviderService.GetCurrentDate();
	card->RecordReview(request.remembered, today);
	card->SetNextReviewDate(today.AddDays(card->GetIntervalDays()));
	if (!this->deckDBService.UpdateCard(*card)) {
		return response;
	}

	std::optional<DailyProgress> progress = this->dailyProgressDBService.GetDailyProgress(today);
	if (!progress.has_value()) {
		progress = DailyProgress(today);
		progress->RecordCard(request.remembered);
		this->dailyProgressDBService.AddDailyProgress(*progress);
	} else {
		progress->RecordCard(request.remembered);
		this->dailyProgressDBService.UpdateDailyProgress(*progress);
	}

	response.success = true;
	response.intervalDays = card->GetIntervalDays();
	response.nextReviewDate = card->GetNextReviewDate()->ToIso();
	response.cardsReviewed = progress->GetCardsReviewed();
	response.cardsCorrect = progress->GetCardsCorrect();
	response.retentionRate = progress->RetentionRate();
	return response;
}
