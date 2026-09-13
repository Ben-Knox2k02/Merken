#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/UpdateEvent/update_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_app_settings_service.h"

TEST_CASE("UpdateEventUseCase updates an existing event") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(7, "gcal-7", "Old title", "2026-09-12T17:00:00", "2026-09-12T17:30:00");
	FakeAppSettingsService appSettings;
	appSettings.settings.calendarApiKey = "cal-key";
	appSettings.settings.aiApiKey = "ai-key";
	UpdateEventUseCase useCase(calendarApi, appSettings);

	UpdateEventRequest request;
	request.calendarEventId = 7;
	request.googleEventId = "gcal-7";
	request.title = "Study Spanish";
	request.description = "Review vocab";
	request.startTime = "2026-09-12T18:00:00";
	request.endTime = "2026-09-12T19:00:00";
	request.reminderMinutes = 15;

	CHECK(useCase.Execute(request));
	REQUIRE(calendarApi.updatedEvents.size() == 1);
	const CalendarEvent& sent = calendarApi.updatedEvents[0];
	CHECK(sent.GetCalendarEventId() == 7);
	CHECK(sent.GetGoogleEventId() == "gcal-7");
	CHECK(sent.GetTitle() == "Study Spanish");
	CHECK(sent.GetDescription() == "Review vocab");
	CHECK(sent.GetStartTime() == "2026-09-12T18:00:00");
	CHECK(sent.GetEndTime() == "2026-09-12T19:00:00");
	CHECK(sent.GetReminderMinutes() == 15);
	CHECK(calendarApi.lastApiKey == "cal-key");
}

TEST_CASE("UpdateEventUseCase uses the default reminder when none is set") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(1, "gcal-1", "Quiz", "2026-09-13T08:00:00", "2026-09-13T08:30:00");
	FakeAppSettingsService appSettings;
	UpdateEventUseCase useCase(calendarApi, appSettings);

	UpdateEventRequest request;
	request.calendarEventId = 1;
	request.googleEventId = "gcal-1";
	request.title = "Quiz";
	request.startTime = "2026-09-13T09:00:00";
	request.endTime = "2026-09-13T09:30:00";

	CHECK(useCase.Execute(request));
	REQUIRE(calendarApi.updatedEvents.size() == 1);
	CHECK(calendarApi.updatedEvents[0].GetReminderMinutes() == 60);
}

TEST_CASE("UpdateEventUseCase returns false when the event is missing") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	UpdateEventUseCase useCase(calendarApi, appSettings);

	UpdateEventRequest request;
	request.calendarEventId = 1;
	request.googleEventId = "gcal-missing";
	request.title = "Quiz";
	request.startTime = "2026-09-13T09:00:00";
	request.endTime = "2026-09-13T09:30:00";

	CHECK_FALSE(useCase.Execute(request));
	CHECK(calendarApi.updatedEvents.empty());
}

TEST_CASE("UpdateEventUseCase returns false when the Google event id is empty") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	UpdateEventUseCase useCase(calendarApi, appSettings);

	UpdateEventRequest request;
	request.calendarEventId = 1;
	request.title = "Quiz";
	request.startTime = "2026-09-13T09:00:00";
	request.endTime = "2026-09-13T09:30:00";

	CHECK_FALSE(useCase.Execute(request));
	CHECK(calendarApi.updatedEvents.empty());
}

TEST_CASE("UpdateEventUseCase returns false when the calendar API update fails") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(1, "gcal-1", "Quiz", "2026-09-13T09:00:00", "2026-09-13T09:30:00");
	calendarApi.updateEventSucceeds = false;
	FakeAppSettingsService appSettings;
	UpdateEventUseCase useCase(calendarApi, appSettings);

	UpdateEventRequest request;
	request.calendarEventId = 1;
	request.googleEventId = "gcal-1";
	request.title = "Quiz";
	request.startTime = "2026-09-13T09:00:00";
	request.endTime = "2026-09-13T09:30:00";

	CHECK_FALSE(useCase.Execute(request));
}
