#include "get_progress_history_usecase.h"
#include <optional>
#include <vector>

GetProgressHistoryResponse GetProgressHistoryUseCase::Execute(const GetProgressHistoryRequest& request) {
	GetProgressHistoryResponse response;

	const Date today = this->dateProviderService.GetCurrentDate();
	Date startDate = today.AddDays(-83);
	Date endDate = today;

	if (!request.startDate.empty()) {
		std::optional<Date> parsedStart = Date::Parse(request.startDate);
		if (!parsedStart.has_value()) {
			return response;
		}
		startDate = *parsedStart;
	}

	if (!request.endDate.empty()) {
		std::optional<Date> parsedEnd = Date::Parse(request.endDate);
		if (!parsedEnd.has_value()) {
			return response;
		}
		endDate = *parsedEnd;
	}

	if (startDate > endDate) {
		return response;
	}

	const std::vector<DailyProgress> stored =
		this->dailyProgressDBService.GetDailyProgressRange(startDate, endDate);

	size_t storedIndex = 0;
	Date day = startDate;
	while (day <= endDate) {
		DailyProgress progress(day);
		while (storedIndex < stored.size() && stored[storedIndex].GetDate() < day) {
			storedIndex += 1;
		}
		if (storedIndex < stored.size() && stored[storedIndex].GetDate() == day) {
			progress = stored[storedIndex];
		}

		response.days.push_back(DailyProgressResponse{
			progress.GetDate().ToIso(),
			progress.GetCardsReviewed(),
			progress.GetCardsCorrect(),
			progress.RetentionRate()
		});

		const Date next = day.AddDays(1);
		if (next <= day) {
			break;
		}
		day = next;
	}

	return response;
}
