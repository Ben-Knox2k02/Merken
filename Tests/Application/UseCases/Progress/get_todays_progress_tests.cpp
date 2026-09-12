#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Progress/GetTodaysProgress/get_todays_progress_usecase.h"
#include "../../../Fakes/fake_daily_progress_db_service.h"
#include "../../../Fakes/fake_date_provider_service.h"

namespace {
Date TestDate() {
	std::optional<Date> date = Date::Create(2026, 9, 12);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("GetTodaysProgressUseCase returns zeros when there is no row for today") {
	FakeDailyProgressDbService progressDb;
	FakeDateProviderService dates(TestDate());
	GetTodaysProgressUseCase useCase(progressDb, dates);

	GetTodaysProgressResponse response = useCase.Execute();

	CHECK(response.date == "2026-09-12");
	CHECK(response.cardsReviewed == 0);
	CHECK(response.cardsCorrect == 0);
	CHECK(response.retentionRate == 0.0);
	CHECK(progressDb.addCount == 0);
	CHECK(progressDb.updateCount == 0);
	CHECK(progressDb.rows.empty());
}

TEST_CASE("GetTodaysProgressUseCase returns stored counts and retention for today") {
	FakeDailyProgressDbService progressDb;
	progressDb.rows.push_back(DailyProgress(TestDate(), 4, 3));
	FakeDateProviderService dates(TestDate());
	GetTodaysProgressUseCase useCase(progressDb, dates);

	GetTodaysProgressResponse response = useCase.Execute();

	CHECK(response.date == "2026-09-12");
	CHECK(response.cardsReviewed == 4);
	CHECK(response.cardsCorrect == 3);
	CHECK(response.retentionRate == doctest::Approx(0.75));
	CHECK(progressDb.addCount == 0);
	CHECK(progressDb.updateCount == 0);
}

TEST_CASE("GetTodaysProgressUseCase ignores progress from other days") {
	FakeDailyProgressDbService progressDb;
	std::optional<Date> yesterday = Date::Create(2026, 9, 11);
	REQUIRE(yesterday.has_value());
	progressDb.rows.push_back(DailyProgress(*yesterday, 10, 9));
	FakeDateProviderService dates(TestDate());
	GetTodaysProgressUseCase useCase(progressDb, dates);

	GetTodaysProgressResponse response = useCase.Execute();

	CHECK(response.date == "2026-09-12");
	CHECK(response.cardsReviewed == 0);
	CHECK(response.cardsCorrect == 0);
	CHECK(response.retentionRate == 0.0);
}
