#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Progress/GetProgressHistory/get_progress_history_usecase.h"
#include "../../../Fakes/fake_daily_progress_db_service.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 12);
	REQUIRE(date.has_value());
	return *date;
}

Date MustParse(const std::string& iso) {
	std::optional<Date> date = Date::Parse(iso);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("GetProgressHistoryUseCase defaults to the last 12 weeks ending today") {
	FakeDailyProgressDbService progressDb;
	FakeDateProviderService dates(TestDate());
	GetProgressHistoryUseCase useCase(progressDb, dates);

	GetProgressHistoryRequest request;
	GetProgressHistoryResponse response = useCase.Execute(request);

	REQUIRE(response.days.size() == 84);
	CHECK(response.days.front().date == "2026-06-21");
	CHECK(response.days.back().date == "2026-09-12");
	CHECK(response.days.front().cardsReviewed == 0);
	CHECK(response.days.back().retentionRate == 0.0);
	REQUIRE(progressDb.lastRangeStart.has_value());
	REQUIRE(progressDb.lastRangeEnd.has_value());
	CHECK(*progressDb.lastRangeStart == MustParse("2026-06-21"));
	CHECK(*progressDb.lastRangeEnd == TestDate());
	CHECK(progressDb.addCount == 0);
	CHECK(progressDb.updateCount == 0);
}

TEST_CASE("GetProgressHistoryUseCase fills missing days and maps stored rows") {
	FakeDailyProgressDbService progressDb;
	progressDb.rows.push_back(DailyProgress(MustParse("2026-09-11"), 4, 3));
	progressDb.rows.push_back(DailyProgress(MustParse("2026-09-01"), 8, 8));
	FakeDateProviderService dates(TestDate());
	GetProgressHistoryUseCase useCase(progressDb, dates);

	GetProgressHistoryRequest request;
	request.startDate = "2026-09-10";
	request.endDate = "2026-09-12";
	GetProgressHistoryResponse response = useCase.Execute(request);

	REQUIRE(response.days.size() == 3);
	CHECK(response.days[0].date == "2026-09-10");
	CHECK(response.days[0].cardsReviewed == 0);
	CHECK(response.days[0].cardsCorrect == 0);
	CHECK(response.days[0].retentionRate == 0.0);
	CHECK(response.days[1].date == "2026-09-11");
	CHECK(response.days[1].cardsReviewed == 4);
	CHECK(response.days[1].cardsCorrect == 3);
	CHECK(response.days[1].retentionRate == doctest::Approx(0.75));
	CHECK(response.days[2].date == "2026-09-12");
	CHECK(response.days[2].cardsReviewed == 0);
	CHECK(progressDb.addCount == 0);
}

TEST_CASE("GetProgressHistoryUseCase returns empty when dates are invalid or reversed") {
	FakeDailyProgressDbService progressDb;
	FakeDateProviderService dates(TestDate());
	GetProgressHistoryUseCase useCase(progressDb, dates);

	GetProgressHistoryRequest badStart;
	badStart.startDate = "not-a-date";
	badStart.endDate = "2026-09-12";
	CHECK(useCase.Execute(badStart).days.empty());

	GetProgressHistoryRequest badEnd;
	badEnd.startDate = "2026-09-10";
	badEnd.endDate = "2026/09/12";
	CHECK(useCase.Execute(badEnd).days.empty());

	GetProgressHistoryRequest reversed;
	reversed.startDate = "2026-09-12";
	reversed.endDate = "2026-09-10";
	CHECK(useCase.Execute(reversed).days.empty());
	CHECK_FALSE(progressDb.lastRangeStart.has_value());
}

TEST_CASE("GetProgressHistoryUseCase includes the max date without hanging") {
	FakeDailyProgressDbService progressDb;
	FakeDateProviderService dates(TestDate());
	GetProgressHistoryUseCase useCase(progressDb, dates);

	GetProgressHistoryRequest request;
	request.startDate = "9999-12-31";
	request.endDate = "9999-12-31";
	GetProgressHistoryResponse response = useCase.Execute(request);

	REQUIRE(response.days.size() == 1);
	CHECK(response.days[0].date == "9999-12-31");
	CHECK(response.days[0].cardsReviewed == 0);
}
