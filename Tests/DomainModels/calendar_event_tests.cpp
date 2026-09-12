#include "doctest/doctest.h"
#include "../../DomainModels/calendar_event.h"

TEST_CASE("CalendarEvent defaults description, reminder, and google id") {
	CalendarEvent event(0, "Study", "2026-09-12T18:00:00", "2026-09-12T19:00:00");
	CHECK(event.GetCalendarEventId() == 0);
	CHECK(event.GetTitle() == "Study");
	CHECK(event.GetDescription() == "");
	CHECK(event.GetStartTime() == "2026-09-12T18:00:00");
	CHECK(event.GetEndTime() == "2026-09-12T19:00:00");
	CHECK(event.GetReminderMinutes() == 60);
	CHECK(event.GetGoogleEventId() == "");
}

TEST_CASE("CalendarEvent fields can be updated") {
	CalendarEvent event(1, "Study", "start", "end");
	event.UpdateTitle("Quiz");
	event.UpdateDescription("Vocab");
	event.UpdateTimeRange("2026-09-13T09:00:00", "2026-09-13T09:30:00");
	event.UpdateReminderMinutes(15);
	event.SetGoogleEventId("gcal-9");

	CHECK(event.GetTitle() == "Quiz");
	CHECK(event.GetDescription() == "Vocab");
	CHECK(event.GetStartTime() == "2026-09-13T09:00:00");
	CHECK(event.GetEndTime() == "2026-09-13T09:30:00");
	CHECK(event.GetReminderMinutes() == 15);
	CHECK(event.GetGoogleEventId() == "gcal-9");
}
