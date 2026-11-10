#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/UpdateEvent/update_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_calendar_auth_service.h"
#include "../../../Fakes/fake_event_repository.h"

namespace {

UpdateEventRequest SampleUpdate(int eventId) {
	UpdateEventRequest request;
	request.calendarEventId = eventId;
	request.title = "Study Spanish";
	request.date = Date::Parse("2026-09-12").value();
	request.description = "Review vocab";
	request.startTime = "18:00";
	request.endTime = "19:00";
	request.location = "Library";
	request.reminderMinutes = 15;
	return request;
}

}

TEST_CASE("UpdateEventUseCase updates the local event and Google when the event has a Google id") {
	FakeEventRepository events;
	const int eventId = events.Seed(
		Date::Parse("2026-09-11").value(),
		"Study session",
		"17:00",
		"17:30",
		"Home",
		"Old",
		30,
		"gcal-7"
	);
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	UpdateEventUseCase useCase(events, calendarApi, auth);

	const UpdateEventRequest request = SampleUpdate(eventId);
	CHECK(useCase.Execute(request));

	REQUIRE(calendarApi.updatedEvents.size() == 1);
	const CalendarEvent& sent = calendarApi.updatedEvents[0];
	CHECK(sent.GetCalendarEventId() == eventId);
	CHECK(sent.GetGoogleEventId() == "gcal-7");
	CHECK(sent.GetTitle() == "Study Spanish");
	CHECK(sent.GetDate() == request.date);
	CHECK(sent.GetDescription() == "Review vocab");
	CHECK(sent.GetStartTime() == "18:00");
	CHECK(sent.GetEndTime() == "19:00");
	CHECK(sent.GetLocation() == "Library");
	CHECK(sent.GetReminderMinutes() == 15);
	CHECK(calendarApi.lastApiKey == "token");

	const std::optional<CalendarEvent> stored = events.GetEvent(eventId);
	REQUIRE(stored.has_value());
	CHECK(stored->GetTitle() == "Study Spanish");
	CHECK(stored->GetLocation() == "Library");
	CHECK(stored->GetGoogleEventId() == "gcal-7");
}

TEST_CASE("UpdateEventUseCase uses the default reminder when none is set") {
	FakeEventRepository events;
	const int eventId = events.Seed(
		Date::Parse("2026-09-13").value(),
		"Vocab quiz",
		"08:00",
		"08:30",
		"",
		"",
		15,
		"gcal-1"
	);
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	UpdateEventUseCase useCase(events, calendarApi, auth);

	UpdateEventRequest request;
	request.calendarEventId = eventId;
	request.title = "Vocab quiz";
	request.date = Date::Parse("2026-09-13").value();
	request.startTime = "09:00";
	request.endTime = "09:30";

	CHECK(useCase.Execute(request));
	REQUIRE(calendarApi.updatedEvents.size() == 1);
	CHECK(calendarApi.updatedEvents[0].GetReminderMinutes() == 60);
	REQUIRE(events.GetEvent(eventId).has_value());
	CHECK(events.GetEvent(eventId)->GetReminderMinutes() == 60);
}

TEST_CASE("UpdateEventUseCase returns false when the event is missing") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	UpdateEventUseCase useCase(events, calendarApi, auth);

	CHECK_FALSE(useCase.Execute(SampleUpdate(1)));
	CHECK(calendarApi.updatedEvents.empty());
}

TEST_CASE("UpdateEventUseCase still saves locally when the Google event id is empty") {
	FakeEventRepository events;
	const int eventId = events.Seed(Date::Parse("2026-09-13").value(), "Vocab quiz", "09:00", "09:30");
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	UpdateEventUseCase useCase(events, calendarApi, auth);

	CHECK(useCase.Execute(SampleUpdate(eventId)));
	CHECK(calendarApi.updatedEvents.empty());
	REQUIRE(events.GetEvent(eventId).has_value());
	CHECK(events.GetEvent(eventId)->GetTitle() == "Study Spanish");
	CHECK(events.GetEvent(eventId)->GetGoogleEventId().empty());
}

TEST_CASE("UpdateEventUseCase still saves locally when the calendar API update fails") {
	FakeEventRepository events;
	const int eventId = events.Seed(
		Date::Parse("2026-09-13").value(),
		"Vocab quiz",
		"09:00",
		"09:30",
		"",
		"",
		60,
		"gcal-1"
	);
	FakeCalendarApiService calendarApi;
	calendarApi.updateEventSucceeds = false;
	FakeCalendarAuthService auth;
	auth.token = "token";
	UpdateEventUseCase useCase(events, calendarApi, auth);

	CHECK(useCase.Execute(SampleUpdate(eventId)));
	REQUIRE(calendarApi.updatedEvents.size() == 1);
	REQUIRE(events.GetEvent(eventId).has_value());
	CHECK(events.GetEvent(eventId)->GetTitle() == "Study Spanish");
}

TEST_CASE("UpdateEventUseCase skips Google when there is no access token") {
	FakeEventRepository events;
	const int eventId = events.Seed(
		Date::Parse("2026-09-13").value(),
		"Vocab quiz",
		"09:00",
		"09:30",
		"",
		"",
		60,
		"gcal-1"
	);
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	UpdateEventUseCase useCase(events, calendarApi, auth);

	CHECK(useCase.Execute(SampleUpdate(eventId)));
	CHECK(calendarApi.updatedEvents.empty());
	REQUIRE(events.GetEvent(eventId).has_value());
	CHECK(events.GetEvent(eventId)->GetTitle() == "Study Spanish");
}
