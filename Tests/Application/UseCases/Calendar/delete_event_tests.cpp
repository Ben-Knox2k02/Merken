#include "doctest/doctest.h"
#include "../../../../Application/UseCases/Calendar/DeleteEvent/delete_event_usecase.h"
#include "../../../Fakes/fake_event_repository.h"

TEST_CASE("DeleteEventUseCase removes a stored event") {
	FakeEventRepository events;
	const int eventId = events.Seed(Date::Parse("2026-09-12").value(), "Study Spanish", "18:00", "19:00");
	DeleteEventUseCase useCase(events);

	DeleteEventRequest request;
	request.calendarEventId = eventId;

	CHECK(useCase.Execute(request));
	CHECK(events.GetEvents().empty());
}

TEST_CASE("DeleteEventUseCase returns false when the event is missing") {
	FakeEventRepository events;
	events.Seed(Date::Parse("2026-09-12").value(), "Study Spanish", "18:00", "19:00");
	DeleteEventUseCase useCase(events);

	DeleteEventRequest request;
	request.calendarEventId = 99;

	CHECK_FALSE(useCase.Execute(request));
	CHECK(events.GetEvents().size() == 1);
}
