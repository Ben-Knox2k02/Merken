#include "doctest/doctest.h"
#include "../../DomainModels/calendar_event.h"

TEST_CASE("CalendarEvent defaults description, reminder, and google id") {
	CalendarEvent event(0, "Study", Date::Create(2026, 9, 12).value(), "18:00", "19:00", "", "");
	CHECK(event.GetCalendarEventId() == 0);
	CHECK(event.GetTitle() == "Study");
	CHECK(event.GetDate() == Date::Create(2026, 9, 12).value());
	CHECK(event.GetDescription() == "");
	CHECK(event.GetLocation() == "");
	CHECK(event.GetStartTime() == "18:00");
	CHECK(event.GetEndTime() == "19:00");
	CHECK(event.GetReminderMinutes() == 60);
	CHECK(event.GetGoogleEventId() == "");
}

TEST_CASE("CalendarEvent fields can be updated") {
	CalendarEvent event(1, "Study Spanish", Date::Create(2026, 9, 12).value(), "18:00", "19:00", "", "");
	event.UpdateTitle("Vocab quiz");
	event.UpdateDate(Date::Create(2026, 9, 13).value());
	event.UpdateDescription("Greetings deck");
	event.UpdateLocation("Library");
	event.UpdateTimeRange("09:00", "09:30");
	event.UpdateReminderMinutes(15);
	event.SetGoogleEventId("gcal-9");

	CHECK(event.GetTitle() == "Vocab quiz");
	CHECK(event.GetDate() == Date::Create(2026, 9, 13).value());
	CHECK(event.GetDescription() == "Greetings deck");
	CHECK(event.GetLocation() == "Library");
	CHECK(event.GetStartTime() == "09:00");
	CHECK(event.GetEndTime() == "09:30");
	CHECK(event.GetReminderMinutes() == 15);
	CHECK(event.GetGoogleEventId() == "gcal-9");
}
