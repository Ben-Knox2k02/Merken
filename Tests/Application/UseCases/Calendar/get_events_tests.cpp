#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include "../../../Fakes/fake_event_repository.h"

TEST_CASE("GetEventsUseCase returns no events when none exist") {
	FakeEventRepository events;
	GetEventsUseCase useCase(events);

	const GetEventsResponse response = useCase.Execute();

	CHECK(response.events.empty());
}

TEST_CASE("GetEventsUseCase maps events from the repository") {
	FakeEventRepository events;
	const Date first = Date::Parse("2026-09-12").value();
	const Date second = Date::Parse("2026-09-13").value();
	events.Seed(first, "Study Spanish", "18:00", "19:00", "Library", "Review vocab", 15, "gcal-7");
	events.Seed(second, "Vocab quiz", "09:00", "09:30");
	GetEventsUseCase useCase(events);

	const GetEventsResponse response = useCase.Execute();

	REQUIRE(response.events.size() == 2);
	CHECK(response.events[0].calendarEventId == 1);
	CHECK(response.events[0].title == "Study Spanish");
	CHECK(response.events[0].date == first);
	CHECK(response.events[0].description == "Review vocab");
	CHECK(response.events[0].startTime == "18:00");
	CHECK(response.events[0].endTime == "19:00");
	CHECK(response.events[0].location == "Library");
	CHECK(response.events[0].reminderMinutes == 15);
	CHECK(response.events[0].googleEventId == "gcal-7");
	CHECK(response.events[1].calendarEventId == 2);
	CHECK(response.events[1].title == "Vocab quiz");
	CHECK(response.events[1].date == second);
	CHECK(response.events[1].description.empty());
	CHECK(response.events[1].startTime == "09:00");
	CHECK(response.events[1].endTime == "09:30");
	CHECK(response.events[1].location.empty());
	CHECK(response.events[1].reminderMinutes == 60);
	CHECK(response.events[1].googleEventId.empty());
}

TEST_CASE("GetEventsUseCase filters by date") {
	FakeEventRepository events;
	const Date first = Date::Parse("2026-09-12").value();
	const Date second = Date::Parse("2026-09-13").value();
	events.Seed(first, "Study Spanish", "18:00", "19:00");
	events.Seed(second, "Vocab quiz", "09:00", "09:30");
	GetEventsUseCase useCase(events);

	const GetEventsResponse response = useCase.ExecuteForDate(second);

	REQUIRE(response.events.size() == 1);
	CHECK(response.events[0].title == "Vocab quiz");
	CHECK(response.events[0].date == second);
}
