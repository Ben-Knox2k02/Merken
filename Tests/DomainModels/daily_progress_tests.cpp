#include "doctest/doctest.h"
#include "../../DomainModels/daily_progress.h"

namespace {
Date MustCreate(int year, int month, int day) {
	std::optional<Date> date = Date::Create(year, month, day);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("DailyProgress defaults to zero counts") {
	DailyProgress progress(MustCreate(2026, 9, 12));
	CHECK(progress.GetDate() == MustCreate(2026, 9, 12));
	CHECK(progress.GetCardsReviewed() == 0);
	CHECK(progress.GetCardsCorrect() == 0);
	CHECK(progress.RetentionRate() == 0.0);
}

TEST_CASE("DailyProgress::RecordCard updates counts and retention") {
	DailyProgress progress(MustCreate(2026, 9, 12), 1, 1);
	progress.RecordCard(true);
	progress.RecordCard(false);

	CHECK(progress.GetCardsReviewed() == 3);
	CHECK(progress.GetCardsCorrect() == 2);
	CHECK(progress.RetentionRate() == doctest::Approx(2.0 / 3.0));
}

TEST_CASE("DailyProgress::RetentionRate is zero when nothing was reviewed") {
	DailyProgress progress(MustCreate(2026, 9, 12), 0, 5);
	CHECK(progress.RetentionRate() == 0.0);
}
