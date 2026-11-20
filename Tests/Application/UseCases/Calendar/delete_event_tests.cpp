#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/DeleteEvent/delete_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_calendar_auth_service.h"
#include "../../../Fakes/fake_event_repository.h"

TEST_CASE("DeleteEventUseCase removes a stored event") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	const int eventId = events.Seed(Date::Parse("2026-09-12").value(), "Study Spanish", "18:00", "19:00");
	DeleteEventUseCase useCase(events, calendarApi, auth);

	DeleteEventRequest request;
	request.calendarEventId = eventId;

	const DeleteEventResponse response = useCase.Execute(request);
	CHECK(response.deleted);
	CHECK_FALSE(response.googleFailed);
	CHECK(events.GetEvents().empty());
	CHECK(calendarApi.deletedGoogleEventIds.empty());
}

TEST_CASE("DeleteEventUseCase returns false when the event is missing") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	events.Seed(Date::Parse("2026-09-12").value(), "Study Spanish", "18:00", "19:00");
	DeleteEventUseCase useCase(events, calendarApi, auth);

	DeleteEventRequest request;
	request.calendarEventId = 99;

	const DeleteEventResponse response = useCase.Execute(request);
	CHECK_FALSE(response.deleted);
	CHECK(events.GetEvents().size() == 1);
	CHECK(calendarApi.deletedGoogleEventIds.empty());
}

TEST_CASE("DeleteEventUseCase deletes the local row before Google") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	const int eventId = events.Seed(
		Date::Parse("2026-09-12").value(),
		"Study Spanish",
		"18:00",
		"19:00",
		"",
		"",
		60,
		"gcal-1"
	);
	DeleteEventUseCase useCase(events, calendarApi, auth);

	DeleteEventRequest request;
	request.calendarEventId = eventId;

	const DeleteEventResponse response = useCase.Execute(request);
	CHECK(response.deleted);
	CHECK_FALSE(response.googleFailed);
	CHECK(events.GetEvents().empty());
	CHECK(calendarApi.deletedGoogleEventIds.size() == 1);
	CHECK(calendarApi.deletedGoogleEventIds[0] == "gcal-1");
	CHECK(calendarApi.lastApiKey == "token-1");
}

TEST_CASE("DeleteEventUseCase keeps the local delete when Google rejects it") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token-1";
	calendarApi.deleteEventSucceeds = false;
	const int eventId = events.Seed(
		Date::Parse("2026-09-12").value(),
		"Study Spanish",
		"18:00",
		"19:00",
		"",
		"",
		60,
		"gcal-1"
	);
	DeleteEventUseCase useCase(events, calendarApi, auth);

	DeleteEventRequest request;
	request.calendarEventId = eventId;

	const DeleteEventResponse response = useCase.Execute(request);
	CHECK(response.deleted);
	CHECK(response.googleFailed);
	CHECK(events.GetEvents().empty());
}

TEST_CASE("DeleteEventUseCase keeps the local delete when Google is unavailable") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.throwOnGetAccessToken = true;
	const int eventId = events.Seed(
		Date::Parse("2026-09-12").value(),
		"Study Spanish",
		"18:00",
		"19:00",
		"",
		"",
		60,
		"gcal-1"
	);
	DeleteEventUseCase useCase(events, calendarApi, auth);

	DeleteEventRequest request;
	request.calendarEventId = eventId;

	const DeleteEventResponse response = useCase.Execute(request);
	CHECK(response.deleted);
	CHECK(response.googleFailed);
	CHECK(events.GetEvents().empty());
	CHECK(calendarApi.deletedGoogleEventIds.empty());
}
