#include "get_todays_progress_usecase.h"
#include <optional>

GetTodaysProgressResponse GetTodaysProgressUseCase::Execute() {
	const Date today = this->dateProviderService.GetCurrentDate();
	std::optional<DailyProgress> progress = this->dailyProgressDBService.GetDailyProgress(today);
	if (!progress.has_value()) {
		progress = DailyProgress(today);
	}

	GetTodaysProgressResponse response;
	response.date = progress->GetDate().ToIso();
	response.cardsReviewed = progress->GetCardsReviewed();
	response.cardsCorrect = progress->GetCardsCorrect();
	response.retentionRate = progress->RetentionRate();
	return response;
}
