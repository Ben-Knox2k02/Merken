#ifndef DATE_H
#define DATE_H

#include <string>
#include <optional>
#include <cstdio>

class Date {
	public:
		static bool IsLeapYear(int year) {
			return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
		}

		static int DaysInMonth(int year, int month) {
			static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
			if (month == 2 && Date::IsLeapYear(year)) {
				return 29;
			}
			if (month < 1 || month > 12) {
				return 0;
			}
			return days[month];
		}

		static bool IsValid(int year, int month, int day) {
			if (year < 1 || year > 9999) {
				return false;
			}
			if (month < 1 || month > 12) {
				return false;
			}
			if (day < 1 || day > Date::DaysInMonth(year, month)) {
				return false;
			}
			return true;
		}

		static std::optional<Date> Create(int year, int month, int day) {
			if (!Date::IsValid(year, month, day)) {
				return std::nullopt;
			}
			return Date(year, month, day);
		}

		static std::optional<Date> Parse(const std::string& iso) {
			if (iso.size() != 10) {
				return std::nullopt;
			}
			if (iso[4] != '-' || iso[7] != '-') {
				return std::nullopt;
			}
			if (!Date::IsDigits(iso, 0, 4) || !Date::IsDigits(iso, 5, 2) || !Date::IsDigits(iso, 8, 2)) {
				return std::nullopt;
			}
			const int year = Date::ParseInt(iso, 0, 4);
			const int month = Date::ParseInt(iso, 5, 2);
			const int day = Date::ParseInt(iso, 8, 2);
			return Date::Create(year, month, day);
		}

		int GetYear() const { return this->year; }
		int GetMonth() const { return this->month; }
		int GetDay() const { return this->day; }

		Date AddDays(int days) const {
			int year = this->year;
			int month = this->month;
			int day = this->day;

			if (days >= 0) {
				day += days;
				while (day > Date::DaysInMonth(year, month)) {
					day -= Date::DaysInMonth(year, month);
					month += 1;
					if (month > 12) {
						month = 1;
						year += 1;
					}
				}
			} else {
				day += days;
				while (day < 1) {
					month -= 1;
					if (month < 1) {
						month = 12;
						year -= 1;
					}
					day += Date::DaysInMonth(year, month);
				}
			}

			std::optional<Date> result = Date::Create(year, month, day);
			if (!result.has_value()) {
				return *this;
			}
			return *result;
		}

		std::string ToIso() const {
			char buf[11] = {0};
			std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", this->year, this->month, this->day);
			return buf;
		}

		bool operator==(const Date& other) const {
			return this->year == other.year
				&& this->month == other.month
				&& this->day == other.day;
		}

		bool operator!=(const Date& other) const {
			return !(*this == other);
		}

		bool operator<(const Date& other) const {
			if (this->year != other.year) {
				return this->year < other.year;
			}
			if (this->month != other.month) {
				return this->month < other.month;
			}
			return this->day < other.day;
		}

		bool operator<=(const Date& other) const {
			return *this < other || *this == other;
		}

		bool operator>(const Date& other) const {
			return other < *this;
		}

		bool operator>=(const Date& other) const {
			return other <= *this;
		}

	private:
		Date(int year, int month, int day)
			: year(year), month(month), day(day) {}

		static bool IsDigits(const std::string& value, size_t start, size_t count) {
			for (size_t i = 0; i < count; ++i) {
				const char c = value[start + i];
				if (c < '0' || c > '9') {
					return false;
				}
			}
			return true;
		}

		static int ParseInt(const std::string& value, size_t start, size_t count) {
			int parsed = 0;
			for (size_t i = 0; i < count; ++i) {
				parsed = parsed * 10 + (value[start + i] - '0');
			}
			return parsed;
		}

		int year;
		int month;
		int day;
};

#endif
