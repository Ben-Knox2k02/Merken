#include "date_provider_service.h"
#include <ctime>
#include <optional>

Date DateProviderService::GetCurrentDate() {
	std::time_t now = std::time(nullptr);
	std::tm* local = std::localtime(&now);
	if (local != nullptr) {
		std::optional<Date> date = Date::Create(
			local->tm_year + 1900,
			local->tm_mon + 1,
			local->tm_mday
		);
		if (date.has_value()) {
			return *date;
		}
	}
	return *Date::Create(1970, 1, 1);
}
