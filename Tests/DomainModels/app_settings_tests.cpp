#include "doctest/doctest.h"
#include "../../DomainModels/app_settings.h"

TEST_CASE("AppSettings defaults to empty keys") {
	AppSettings settings;
	CHECK(settings.calendarApiKey.empty());
	CHECK(settings.aiApiKey.empty());
}

TEST_CASE("AppSettings stores API keys") {
	AppSettings settings;
	settings.calendarApiKey = "cal-key";
	settings.aiApiKey = "ai-key";
	CHECK(settings.calendarApiKey == "cal-key");
	CHECK(settings.aiApiKey == "ai-key");
}
