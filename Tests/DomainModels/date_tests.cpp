#include "doctest/doctest.h"
#include "../../DomainModels/date.h"

namespace doctest {
template <>
struct StringMaker<Date> {
	static String convert(const Date& value) {
		return value.ToIso().c_str();
	}
};
}

namespace {
Date MustCreate(int year, int month, int day) {
	std::optional<Date> date = Date::Create(year, month, day);
	REQUIRE(date.has_value());
	return *date;
}
}

TEST_CASE("Date::IsLeapYear") {
	CHECK(Date::IsLeapYear(2000));
	CHECK(Date::IsLeapYear(2004));
	CHECK(Date::IsLeapYear(4));
	CHECK_FALSE(Date::IsLeapYear(1900));
	CHECK_FALSE(Date::IsLeapYear(2001));
	CHECK_FALSE(Date::IsLeapYear(1));
	CHECK_FALSE(Date::IsLeapYear(2100));
}

TEST_CASE("Date::DaysInMonth") {
	CHECK(Date::DaysInMonth(2026, 1) == 31);
	CHECK(Date::DaysInMonth(2026, 3) == 31);
	CHECK(Date::DaysInMonth(2026, 4) == 30);
	CHECK(Date::DaysInMonth(2026, 11) == 30);
	CHECK(Date::DaysInMonth(2026, 12) == 31);
	CHECK(Date::DaysInMonth(2023, 2) == 28);
	CHECK(Date::DaysInMonth(2024, 2) == 29);
	CHECK(Date::DaysInMonth(1900, 2) == 28);
	CHECK(Date::DaysInMonth(2000, 2) == 29);
	CHECK(Date::DaysInMonth(2026, 0) == 0);
	CHECK(Date::DaysInMonth(2026, 13) == 0);
	CHECK(Date::DaysInMonth(2026, -1) == 0);
}

TEST_CASE("Date::IsValid and Create") {
	CHECK(Date::IsValid(1, 1, 1));
	CHECK(Date::IsValid(9999, 12, 31));
	CHECK(Date::IsValid(2024, 2, 29));
	CHECK(Date::Create(2026, 9, 12).has_value());

	CHECK_FALSE(Date::IsValid(0, 1, 1));
	CHECK_FALSE(Date::IsValid(10000, 1, 1));
	CHECK_FALSE(Date::IsValid(2026, 0, 1));
	CHECK_FALSE(Date::IsValid(2026, 13, 1));
	CHECK_FALSE(Date::IsValid(2026, 1, 0));
	CHECK_FALSE(Date::IsValid(2026, 1, 32));
	CHECK_FALSE(Date::IsValid(2023, 2, 29));
	CHECK_FALSE(Date::IsValid(2026, 4, 31));
	CHECK_FALSE(Date::Create(2023, 2, 29).has_value());
	CHECK_FALSE(Date::Create(0, 1, 1).has_value());
}

TEST_CASE("Date::Parse") {
	std::optional<Date> parsed = Date::Parse("2026-09-12");
	REQUIRE(parsed.has_value());
	CHECK(parsed->GetYear() == 2026);
	CHECK(parsed->GetMonth() == 9);
	CHECK(parsed->GetDay() == 12);

	CHECK(Date::Parse("0001-01-01").has_value());
	CHECK(Date::Parse("9999-12-31").has_value());
	CHECK(Date::Parse("2024-02-29").has_value());

	CHECK_FALSE(Date::Parse("").has_value());
	CHECK_FALSE(Date::Parse("2026-9-12").has_value());
	CHECK_FALSE(Date::Parse("2026/09/12").has_value());
	CHECK_FALSE(Date::Parse("2026-09-12x").has_value());
	CHECK_FALSE(Date::Parse("abcd-09-12").has_value());
	CHECK_FALSE(Date::Parse("2026-00-01").has_value());
	CHECK_FALSE(Date::Parse("2026-13-01").has_value());
	CHECK_FALSE(Date::Parse("2026-01-32").has_value());
	CHECK_FALSE(Date::Parse("2025-02-29").has_value());
	CHECK_FALSE(Date::Parse("0000-01-01").has_value());
}

TEST_CASE("Date::ToIso") {
	CHECK(MustCreate(2026, 9, 4).ToIso() == "2026-09-04");
	CHECK(MustCreate(1, 1, 1).ToIso() == "0001-01-01");
	CHECK(MustCreate(9999, 12, 31).ToIso() == "9999-12-31");
}

TEST_CASE("Date::AddDays") {
	const Date start = MustCreate(2026, 1, 15);
	CHECK(start.AddDays(0) == start);
	CHECK(start.AddDays(1) == MustCreate(2026, 1, 16));
	CHECK(start.AddDays(-1) == MustCreate(2026, 1, 14));

	CHECK(MustCreate(2026, 1, 31).AddDays(1) == MustCreate(2026, 2, 1));
	CHECK(MustCreate(2026, 12, 31).AddDays(1) == MustCreate(2027, 1, 1));
	CHECK(MustCreate(2027, 1, 1).AddDays(-1) == MustCreate(2026, 12, 31));

	CHECK(MustCreate(2024, 2, 28).AddDays(1) == MustCreate(2024, 2, 29));
	CHECK(MustCreate(2024, 2, 29).AddDays(1) == MustCreate(2024, 3, 1));
	CHECK(MustCreate(2023, 2, 28).AddDays(1) == MustCreate(2023, 3, 1));
	CHECK(MustCreate(2024, 3, 1).AddDays(-1) == MustCreate(2024, 2, 29));

	CHECK(MustCreate(2024, 1, 1).AddDays(366) == MustCreate(2025, 1, 1));
	CHECK(MustCreate(2023, 1, 1).AddDays(365) == MustCreate(2024, 1, 1));
	CHECK(MustCreate(2026, 9, 12).AddDays(-83) == MustCreate(2026, 6, 21));

	const Date maxDate = MustCreate(9999, 12, 31);
	CHECK(maxDate.AddDays(1) == maxDate);

	const Date minDate = MustCreate(1, 1, 1);
	CHECK(minDate.AddDays(-1) == minDate);
}

TEST_CASE("Date comparison") {
	const Date earlier = MustCreate(2026, 1, 1);
	const Date mid = MustCreate(2026, 6, 15);
	const Date later = MustCreate(2027, 1, 1);
	const Date same = MustCreate(2026, 6, 15);

	CHECK(mid == same);
	CHECK(mid != earlier);
	CHECK(earlier < mid);
	CHECK(mid < later);
	CHECK(earlier <= mid);
	CHECK(mid <= same);
	CHECK(later > mid);
	CHECK(mid >= same);
	CHECK_FALSE(later < earlier);
	CHECK_FALSE(mid != same);
}
