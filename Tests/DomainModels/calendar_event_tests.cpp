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
	CHECK(event.GetLocation().empty());
	CHECK(event.GetDate() == Date::Create(1970, 1, 1).value());
}

TEST_CASE("CalendarEvent stores a date and location") {
	const Date date = Date::Parse("2026-09-12").value();
	CalendarEvent event(3, "Study", date, "18:00", "19:00", "Library", "Review vocab");
	CHECK(event.GetDate() == date);
	CHECK(event.GetLocation() == "Library");
	CHECK(event.GetDescription() == "Review vocab");
	CHECK(event.GetReminderMinutes() == 60);

	event.UpdateDate(Date::Parse("2026-09-13").value());
	event.UpdateLocation("Home");
	CHECK(event.GetDate() == Date::Parse("2026-09-13").value());
	CHECK(event.GetLocation() == "Home");
}

TEST_CASE("CalendarEvent fields can be updated") {
	CalendarEvent event(1, "Study Spanish", "2026-09-12T18:00:00", "2026-09-12T19:00:00");
	event.UpdateTitle("Vocab quiz");
	event.UpdateDescription("Greetings deck");
	event.UpdateTimeRange("2026-09-13T09:00:00", "2026-09-13T09:30:00");
	event.UpdateReminderMinutes(15);
	event.SetGoogleEventId("gcal-9");

	CHECK(event.GetTitle() == "Vocab quiz");
	CHECK(event.GetDescription() == "Greetings deck");
	CHECK(event.GetStartTime() == "2026-09-13T09:00:00");
	CHECK(event.GetEndTime() == "2026-09-13T09:30:00");
	CHECK(event.GetReminderMinutes() == 15);
	CHECK(event.GetGoogleEventId() == "gcal-9");
}
