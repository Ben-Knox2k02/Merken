#include "get_todays_progress_usecase.h"
#include <optional>

GetTodaysProgressResponse GetTodaysProgressUseCase::Execute() {
	const Date today = this->dateProviderService.GetCurrentDate();
	std::optional<DailyProgress> progress = this->dailyProgressRepository.GetDailyProgress(today);
	if (!progress.has_value()) {
		progress = DailyProgress(today);
	}
	
	return GetTodaysProgressResponse{
		.date = progress->GetDate().ToIso(),
		.cardsReviewed = progress->GetCardsReviewed(),
		.cardsCorrect = progress->GetCardsCorrect(),
		.retentionRate = progress->RetentionRate()
	};
}
