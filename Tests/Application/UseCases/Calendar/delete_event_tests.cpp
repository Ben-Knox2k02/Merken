#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/DeleteEvent/delete_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_calendar_auth_service.h"

TEST_CASE("DeleteEventUseCase deletes an existing event using the access token") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(7, "gcal-7", "Study session", "17:00", "17:30");
	FakeCalendarAuthService auth;
	auth.accessToken = "cal-key";
	DeleteEventUseCase useCase(calendarApi, auth);

	DeleteEventRequest request;
	request.id = "gcal-7";

	CHECK(useCase.Execute(request));
	REQUIRE(calendarApi.deletedEventIds.size() == 1);
	CHECK(calendarApi.deletedEventIds[0] == "gcal-7");
	CHECK(calendarApi.events.empty());
	CHECK(calendarApi.lastApiKey == "cal-key");
}

TEST_CASE("DeleteEventUseCase returns false when the Google event id is empty") {
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	DeleteEventUseCase useCase(calendarApi, auth);

	CHECK_FALSE(useCase.Execute(DeleteEventRequest{}));
}

TEST_CASE("DeleteEventUseCase returns false when the calendar API delete fails") {
	FakeCalendarApiService calendarApi;
	calendarApi.SeedEvent(1, "gcal-1", "Vocab quiz", "09:00", "09:30");
	calendarApi.deleteEventSucceeds = false;
	FakeCalendarAuthService auth;
	DeleteEventUseCase useCase(calendarApi, auth);

	DeleteEventRequest request;
	request.id = "gcal-1";

	CHECK_FALSE(useCase.Execute(request));
	CHECK(calendarApi.events.size() == 1);
}