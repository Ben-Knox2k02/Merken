#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/CreateEvent/create_event_usecase.h"
#include "../../../Fakes/fake_calendar_api_service.h"
#include "../../../Fakes/fake_calendar_auth_service.h"
#include "../../../Fakes/fake_event_repository.h"

namespace {

CreateEventRequest SampleRequest() {
	CreateEventRequest request;
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

TEST_CASE("CreateEventUseCase stores the event locally and creates it in Google when signed in") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	CreateEventUseCase useCase(events, calendarApi, auth);

	const CreateEventRequest request = SampleRequest();
	const CreateEventResponse response = useCase.Execute(request);

	CHECK(response.calendarEventId == 1);
	CHECK(response.title == "Study Spanish");
	CHECK(response.date == request.date);
	CHECK(response.description == "Review vocab");
	CHECK(response.startTime == "18:00");
	CHECK(response.endTime == "19:00");
	CHECK(response.location == "Library");
	CHECK(response.reminderMinutes == 15);
	CHECK(response.googleEventId == "gcal-1");
	CHECK(calendarApi.lastApiKey == "token");
	REQUIRE(calendarApi.createdEvents.size() == 1);
	CHECK(calendarApi.createdEvents[0].GetTitle() == "Study Spanish");
	CHECK(calendarApi.createdEvents[0].GetStartTime() == "18:00");
	CHECK(calendarApi.createdEvents[0].GetReminderMinutes() == 15);

	const std::optional<CalendarEvent> stored = events.GetEvent(1);
	REQUIRE(stored.has_value());
	CHECK(stored->GetGoogleEventId() == "gcal-1");
	CHECK(stored->GetDate() == request.date);
	CHECK(stored->GetLocation() == "Library");
	CHECK(stored->GetReminderMinutes() == 15);
}

TEST_CASE("CreateEventUseCase keeps the local event when Google fails") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	auth.throwOnGetAccessToken = true;
	CreateEventUseCase useCase(events, calendarApi, auth);

	const CreateEventResponse response = useCase.Execute(SampleRequest());

	CHECK(response.calendarEventId == 1);
	CHECK(response.googleEventId.empty());
	CHECK(calendarApi.createdEvents.empty());
	REQUIRE(events.GetEvent(1).has_value());
	CHECK(events.GetEvent(1)->GetTitle() == "Study Spanish");
	CHECK(events.GetEvent(1)->GetGoogleEventId().empty());
}

TEST_CASE("CreateEventUseCase saves locally and skips Google when there is no access token") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	CreateEventUseCase useCase(events, calendarApi, auth);

	const CreateEventResponse response = useCase.Execute(SampleRequest());

	CHECK(response.calendarEventId == 1);
	CHECK(response.googleEventId.empty());
	CHECK(calendarApi.createdEvents.empty());
	CHECK(calendarApi.lastApiKey.empty());
	REQUIRE(events.GetEvent(1).has_value());
	CHECK(events.GetEvent(1)->GetTitle() == "Study Spanish");
	CHECK(events.GetEvent(1)->GetGoogleEventId().empty());
}

TEST_CASE("CreateEventUseCase uses the default reminder when none is set") {
	FakeEventRepository events;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	CreateEventUseCase useCase(events, calendarApi, auth);

	CreateEventRequest request;
	request.title = "Vocab quiz";
	request.date = Date::Parse("2026-09-13").value();
	request.startTime = "09:00";
	request.endTime = "09:30";

	const CreateEventResponse response = useCase.Execute(request);

	CHECK(response.reminderMinutes == 60);
	CHECK(response.calendarEventId == 1);
	CHECK(calendarApi.createdEvents.empty());
	REQUIRE(events.GetEvent(1).has_value());
	CHECK(events.GetEvent(1)->GetReminderMinutes() == 60);
}

TEST_CASE("CreateEventUseCase does not call Google when the local save fails") {
	FakeEventRepository events;
	events.addSucceeds = false;
	FakeCalendarApiService calendarApi;
	FakeCalendarAuthService auth;
	auth.token = "token";
	CreateEventUseCase useCase(events, calendarApi, auth);

	const CreateEventResponse response = useCase.Execute(SampleRequest());

	CHECK(response.calendarEventId == 0);
	CHECK(response.googleEventId.empty());
	CHECK(calendarApi.createdEvents.empty());
	CHECK(events.GetEvents().empty());
}
