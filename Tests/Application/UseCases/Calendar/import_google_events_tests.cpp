#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/ImportGoogleEvents/import_google_events_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_calendar_auth_service.h"
#include "../../../Fakes/fake_event_repository.h"

namespace {

CalendarEvent RemoteEvent(
	const std::string& googleEventId,
	const std::string& title,
	const std::string& date = "2026-09-12"
) {
	CalendarEvent event(
		0,
		title,
		Date::Parse(date).value(),
		"18:00",
		"19:00",
		"Library",
		"Review vocab"
	);
	event.UpdateReminderMinutes(15);
	event.SetGoogleEventId(googleEventId);
	return event;
}

}

TEST_CASE("ImportGoogleEventsUseCase inserts a Google event that is not stored") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 1);
	CHECK_FALSE(response.googleFailed);
	CHECK(calendarApi.lastApiKey == "token-1");
	const std::vector<CalendarEvent> storedEvents = events.GetEvents();
	REQUIRE(storedEvents.size() == 1);
	const CalendarEvent& stored = storedEvents[0];
	CHECK(stored.GetCalendarEventId() == 1);
	CHECK(stored.GetGoogleEventId() == "gcal-1");
	CHECK(stored.GetTitle() == "Quiz");
	CHECK(stored.GetDate() == Date::Parse("2026-09-12").value());
	CHECK(stored.GetStartTime() == "18:00");
	CHECK(stored.GetEndTime() == "19:00");
	CHECK(stored.GetLocation() == "Library");
	CHECK(stored.GetDescription() == "Review vocab");
	CHECK(stored.GetReminderMinutes() == 15);
}

TEST_CASE("ImportGoogleEventsUseCase does not change a stored Google event that is identical") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	events.Seed(
		Date::Parse("2026-09-12").value(),
		"Quiz",
		"18:00",
		"19:00",
		"Library",
		"Review vocab",
		15,
		"gcal-1"
	);
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 0);
	CHECK(response.updated == 0);
	CHECK_FALSE(response.googleFailed);
	CHECK(events.GetEvents().size() == 1);
}

TEST_CASE("ImportGoogleEventsUseCase updates a stored Google event that was edited on Google") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	const int eventId = events.Seed(
		Date::Parse("2026-09-12").value(),
		"Local title",
		"09:00",
		"10:00",
		"Home",
		"Keep this",
		30,
		"gcal-1"
	);
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Google title", "2026-10-01"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 0);
	CHECK(response.updated == 1);
	CHECK_FALSE(response.googleFailed);
	REQUIRE(events.GetEvents().size() == 1);
	const std::optional<CalendarEvent> stored = events.GetEvent(eventId);
	REQUIRE(stored.has_value());
	CHECK(stored->GetGoogleEventId() == "gcal-1");
	CHECK(stored->GetTitle() == "Google title");
	CHECK(stored->GetStartTime() == "18:00");
	CHECK(stored->GetLocation() == "Library");
	CHECK(stored->GetDescription() == "Review vocab");
	CHECK(stored->GetReminderMinutes() == 15);
	CHECK(stored->GetDate() == Date::Parse("2026-10-01").value());
}

TEST_CASE("ImportGoogleEventsUseCase keeps local events that are not on Google") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	events.Seed(Date::Parse("2026-09-12").value(), "Local only", "09:00", "10:00");
	events.Seed(
		Date::Parse("2026-09-11").value(),
		"Already linked",
		"08:00",
		"09:00",
		"",
		"",
		60,
		"gcal-old"
	);
	calendarApi.events.push_back(RemoteEvent("gcal-new", "From Google"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 1);
	CHECK(events.GetEvents().size() == 3);
	CHECK(events.GetEvents()[0].GetTitle() == "Local only");
	CHECK(events.GetEvents()[0].GetGoogleEventId().empty());
	CHECK(events.GetEvents()[1].GetGoogleEventId() == "gcal-old");
	CHECK(events.GetEvents()[2].GetGoogleEventId() == "gcal-new");
}

TEST_CASE("ImportGoogleEventsUseCase skips a Google event with no id") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	calendarApi.events.push_back(RemoteEvent("", "No id"));
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz again"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 1);
	REQUIRE(events.GetEvents().size() == 1);
	CHECK(events.GetEvents()[0].GetGoogleEventId() == "gcal-1");
	CHECK(events.GetEvents()[0].GetTitle() == "Quiz");
}

TEST_CASE("ImportGoogleEventsUseCase does not call Google when signed out") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	events.Seed(Date::Parse("2026-09-12").value(), "Local only", "09:00", "10:00");
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 0);
	CHECK_FALSE(response.googleFailed);
	CHECK(calendarApi.lastApiKey.empty());
	CHECK(events.GetEvents().size() == 1);
	CHECK(events.GetEvents()[0].GetTitle() == "Local only");
}

TEST_CASE("ImportGoogleEventsUseCase leaves local events when Google is unavailable") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	auth.throwOnGetAccessToken = true;
	events.Seed(Date::Parse("2026-09-12").value(), "Local only", "09:00", "10:00");
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 0);
	CHECK(response.googleFailed);
	CHECK(calendarApi.lastApiKey.empty());
	CHECK(events.GetEvents().size() == 1);
	CHECK(events.GetEvents()[0].GetTitle() == "Local only");
}

TEST_CASE("ImportGoogleEventsUseCase keeps local events when the new row cannot be saved") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	events.Seed(Date::Parse("2026-09-12").value(), "Local only", "09:00", "10:00");
	events.addSucceeds = false;
	calendarApi.events.push_back(RemoteEvent("gcal-1", "Quiz"));
	ImportGoogleEventsUseCase useCase(events, calendarApi, auth);

	const ImportGoogleEventsResponse response = useCase.Execute();

	CHECK(response.imported == 0);
	CHECK_FALSE(response.googleFailed);
	CHECK(events.GetEvents().size() == 1);
	CHECK(events.GetEvents()[0].GetTitle() == "Local only");
}
