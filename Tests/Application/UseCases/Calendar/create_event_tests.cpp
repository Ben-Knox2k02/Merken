#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/CreateEvent/create_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_app_settings_service.h"

TEST_CASE("CreateEventUseCase sends the event and settings to the calendar API") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	appSettings.settings.calendarApiKey = "cal-key";
	appSettings.settings.aiApiKey = "ai-key";
	CreateEventUseCase useCase(calendarApi, appSettings);

	CreateEventRequest request;
	request.title = "Study Spanish";
	request.description = "Review vocab";
	request.startTime = "2026-09-12T18:00:00";
	request.endTime = "2026-09-12T19:00:00";
	request.reminderMinutes = 15;

	CreateEventResponse response = useCase.Execute(request);

	REQUIRE(calendarApi.createdEvents.size() == 1);
	const CalendarEvent& sent = calendarApi.createdEvents[0];
	CHECK(sent.GetCalendarEventId() == 0);
	CHECK(sent.GetTitle() == "Study Spanish");
	CHECK(sent.GetDescription() == "Review vocab");
	CHECK(sent.GetStartTime() == "2026-09-12T18:00:00");
	CHECK(sent.GetEndTime() == "2026-09-12T19:00:00");
	CHECK(sent.GetReminderMinutes() == 15);
	CHECK(calendarApi.lastSettings.calendarApiKey == "cal-key");
	CHECK(calendarApi.lastSettings.aiApiKey == "ai-key");

	CHECK(response.calendarEventId == 1);
	CHECK(response.title == "Study Spanish");
	CHECK(response.description == "Review vocab");
	CHECK(response.startTime == "2026-09-12T18:00:00");
	CHECK(response.endTime == "2026-09-12T19:00:00");
	CHECK(response.reminderMinutes == 15);
	CHECK(response.googleEventId == "gcal-1");
}

TEST_CASE("CreateEventUseCase uses the default reminder when none is set") {
	FakeCalendarApiService calendarApi;
	FakeAppSettingsService appSettings;
	CreateEventUseCase useCase(calendarApi, appSettings);

	CreateEventRequest request;
	request.title = "Quiz";
	request.startTime = "2026-09-13T09:00:00";
	request.endTime = "2026-09-13T09:30:00";

	CreateEventResponse response = useCase.Execute(request);

	REQUIRE(calendarApi.createdEvents.size() == 1);
	CHECK(calendarApi.createdEvents[0].GetReminderMinutes() == 60);
	CHECK(response.reminderMinutes == 60);
	CHECK(response.calendarEventId == 1);
}
