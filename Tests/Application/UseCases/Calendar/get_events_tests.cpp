#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_app_settings_service.h"

TEST_CASE("GetEventsUseCase returns no events when none exist") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	appSettings.settings.calendarApiKey = "cal-key";
	GetEventsUseCase useCase(calendarApi, appSettings);

	GetEventsResponse response = useCase.Execute();

	CHECK(response.events.empty());
	CHECK(calendarApi.lastApiKey == "cal-key");
}

TEST_CASE("GetEventsUseCase maps calendar events") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(
		7,
		"gcal-7",
		"Study Spanish",
		"2026-09-12T18:00:00",
		"2026-09-12T19:00:00",
		"Review vocab",
		15);
	calendarApi.SeedEvent(8, "gcal-8", "Vocab quiz", "2026-09-13T09:00:00", "2026-09-13T09:30:00");
	FakeAppSettingsService appSettings;
	appSettings.settings.calendarApiKey = "cal-key";
	GetEventsUseCase useCase(calendarApi, appSettings);

	GetEventsResponse response = useCase.Execute();

	REQUIRE(response.events.size() == 2);
	CHECK(response.events[0].calendarEventId == 7);
	CHECK(response.events[0].title == "Study Spanish");
	CHECK(response.events[0].description == "Review vocab");
	CHECK(response.events[0].startTime == "2026-09-12T18:00:00");
	CHECK(response.events[0].endTime == "2026-09-12T19:00:00");
	CHECK(response.events[0].reminderMinutes == 15);
	CHECK(response.events[0].googleEventId == "gcal-7");
	CHECK(response.events[1].calendarEventId == 8);
	CHECK(response.events[1].title == "Vocab quiz");
	CHECK(response.events[1].description == "");
	CHECK(response.events[1].startTime == "2026-09-13T09:00:00");
	CHECK(response.events[1].endTime == "2026-09-13T09:30:00");
	CHECK(response.events[1].reminderMinutes == 60);
	CHECK(response.events[1].googleEventId == "gcal-8");
	CHECK(calendarApi.lastApiKey == "cal-key");
}

TEST_CASE("GetEventsUseCase uses the calendar API key from settings") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	appSettings.settings.calendarApiKey = "cal-key";
	appSettings.settings.aiApiKey = "ai-key";
	GetEventsUseCase useCase(calendarApi, appSettings);

	useCase.Execute();

	CHECK(calendarApi.lastApiKey == "cal-key");
}
