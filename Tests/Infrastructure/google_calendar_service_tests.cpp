#include "doctest/doctest.h"
#include <optional>
#include <string>
#include "../Fakes/fake_http_client.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include "../../Infrastructure/GoogleCalendar/google_calendar_service.h"


static const std::string kBaseUrl = "https://www.googleapis.com/calendar/v3/calendars/primary/events";

static CalendarEvent MakeEvent(){
    CalendarEvent event(0, "Study Session", Date::Create(2026, 10, 14).value(), "10:00", "12:00", "Library", "Review Biology Deck");
    event.UpdateReminderMinutes(15);
    return event;
}

TEST_CASE("GoogleCalendarService CreateEvent stores the Google event id on success"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123","status":"confirmed","summary":"Study Session"})");

    CalendarEvent created = service.CreateEvent(MakeEvent(), "token");
    CHECK(created.GetGoogleEventId() == "abc123");
    CHECK(created.GetTitle() == "Study Session");
    CHECK(http.postCallCount == 1);
    CHECK(http.lastUrl == kBaseUrl);
    CHECK(http.LastHeaderValue("Authorization") == "Bearer token");
    CHECK(http.lastBody.find("\"summary\":\"Study Session\"") != std::string::npos);
}

TEST_CASE("GoogleCalendarService CreateEvent accepts a 201 response"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(201, R"({"id":"xyz789"})");

    CalendarEvent created = service.CreateEvent(MakeEvent(), "token");
    CHECK(created.GetGoogleEventId() == "xyz789");
}

TEST_CASE("GoogleCalendarService CreateEvent returns the original event on a 401"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(401, R"({"error":{"code":401,"message":"Invalid Credentials","status":"UNAUTHENTICATED"}})");

    CalendarEvent original = MakeEvent();
    CalendarEvent created = service.CreateEvent(original, "bad-token");
    CHECK(created.GetGoogleEventId().empty());
    CHECK(created.GetTitle() == original.GetTitle());
}

TEST_CASE("GoogleCalendarService CreateEvent returns the original event when there is no response"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(0, "", "timeout");
    CalendarEvent created = service.CreateEvent(MakeEvent(), "token");
    CHECK(created.GetGoogleEventId().empty());
}

TEST_CASE("GoogleCalendarService CreateEvent returns the original event when a 200 has no id"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"status":"confirmed"})");
    CalendarEvent created = service.CreateEvent(MakeEvent(), "token");
    CHECK(created.GetGoogleEventId().empty());
}

TEST_CASE("GoogleCalendarService CreateEvent does not call the API without a token"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    CalendarEvent created = service.CreateEvent(MakeEvent(), "");
    CHECK(created.GetGoogleEventId().empty());
    CHECK(http.postCallCount == 0);
}

TEST_CASE("GoogleCalendarService GetEvent parses an event and sends the Bearer header"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123","summary":"Exam","start":{"dateTime":"2026-10-14T10:00:00-05:00"},"end":{"dateTime":"2026-10-14T12:00:00-05:00"}})");
    std::optional<CalendarEvent> event = service.GetEvent("abc123", "token");
    REQUIRE(event.has_value());
    CHECK(event->GetGoogleEventId() == "abc123");
    CHECK(event->GetTitle() == "Exam");
    CHECK(http.lastUrl == kBaseUrl + "/abc123");
    CHECK(http.LastHeaderValue("Authorization") == "Bearer token");
}

TEST_CASE("GoogleCalendarService GetEvent returns nullopt on a 404"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(404, R"({"error":{"code":404,"message":"Not Found"}})");
    CHECK_FALSE(service.GetEvent("missing", "token").has_value());
}

TEST_CASE("GoogleCalendarService GetEvents follows pagination and skips cancelled events"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"items":[{"id":"a","summary":"One"},{"id":"b","summary":"Gone","status":"cancelled"}],"nextPageToken":"tok"})");
    http.QueueResponse(200, R"({"items":[{"id":"c","summary":"Two"}]})");

    std::vector<CalendarEvent> events = service.GetEvents("token");
    REQUIRE(events.size() == 2);
    CHECK(events[0].GetGoogleEventId() == "a");
    CHECK(events[1].GetGoogleEventId() == "c");
    REQUIRE(http.requestedUrls.size() == 2);
    CHECK(http.requestedUrls[0] == kBaseUrl);
    CHECK(http.requestedUrls[1] == kBaseUrl + "?pageToken=tok");
}

TEST_CASE("GoogleCalendarService GetEvents returns earlier pages if a later page fails"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"items":[{"id":"a","summary":"One"}],"nextPageToken":"tok"})");
    http.QueueResponse(500, R"({"error":{"code":500,"message":"Backend Error"}})");

    std::vector<CalendarEvent> events = service.GetEvents("token");
    REQUIRE(events.size() == 1);
    CHECK(events[0].GetGoogleEventId() == "a");
}

TEST_CASE("GoogleCalendarService GetEvents returns nothing on a 401"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(401, R"({"error":{"code":401,"message":"Invalid Credentials"}})");
    CHECK(service.GetEvents("bad-token").empty());
}

TEST_CASE("GoogleCalendarService GetEvents stops at the page limit"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    for (int i = 0; i < 100; ++i) {
        http.QueueResponse(200, R"({"items":[],"nextPageToken":"again"})");
    }

    service.GetEvents("token");
    REQUIRE(http.getCallCount == 50);
}

static CalendarEvent MakeSyncedEvent(){
    CalendarEvent event = MakeEvent();
    event.SetGoogleEventId("abc123");
    return event;
}

TEST_CASE("GoogleCalendarService UpdateEvent sends a PATCH and returns true on success"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123","status":"confirmed"})");
    CHECK(service.UpdateEvent(MakeSyncedEvent(), "token"));
    CHECK(http.patchCallCount == 1);
    REQUIRE(http.requestedMethods.size() == 1);
    CHECK(http.requestedMethods[0] == "PATCH");
    CHECK(http.lastUrl == kBaseUrl + "/abc123");
    CHECK(http.LastHeaderValue("Authorization") == "Bearer token");
    CHECK(http.lastBody.find("\"summary\":\"Study Session\"") != std::string::npos);
}

TEST_CASE("GoogleCalendarService UpdateEvent returns false on a 404"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(404, R"({"error":{"code":404,"message":"Not Found"}})");
    CHECK_FALSE(service.UpdateEvent(MakeSyncedEvent(), "token"));
}

TEST_CASE("GoogleCalendarService UpdateEvent returns false on a 401"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(401, R"({"error":{"code":401,"message":"Invalid Credentials"}})");
    CHECK_FALSE(service.UpdateEvent(MakeSyncedEvent(), "bad-token"));
}

TEST_CASE("GoogleCalendarService UpdateEvent returns false when there is no response"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(0, "", "timeout");
    CHECK_FALSE(service.UpdateEvent(MakeSyncedEvent(), "token"));
}

TEST_CASE("GoogleCalendarService UpdateEvent does not call the API without a Google event id"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    CHECK_FALSE(service.UpdateEvent(MakeEvent(), "token"));
    CHECK(http.patchCallCount == 0);
}

TEST_CASE("GoogleCalendarService UpdateEvent does not call the API without a token"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    CHECK_FALSE(service.UpdateEvent(MakeSyncedEvent(), ""));
    CHECK(http.patchCallCount == 0);
}

static std::string JsonField(const std::string& body, const std::string& path1, const std::string& path2){
    nlohmann::json j = nlohmann::json::parse(body);
    return j[path1][path2].get<std::string>();
}

TEST_CASE("GoogleCalendarService CreateEvent sends the date, times and location with a UTC offset"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123"})");

    service.CreateEvent(MakeEvent(), "token");

    const std::string start = JsonField(http.lastBody, "start", "dateTime");
    const std::string end = JsonField(http.lastBody, "end", "dateTime");
    CHECK(start.size() == 25);
    CHECK(start.rfind("2026-10-14T10:00:00", 0) == 0);
    CHECK((start[19] == '+' || start[19] == '-'));
    CHECK(end.rfind("2026-10-14T12:00:00", 0) == 0);
    CHECK(nlohmann::json::parse(http.lastBody)["location"] == "Library");
}

TEST_CASE("GoogleCalendarService CreateEvent accepts 12-hour times"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123"})");
    CalendarEvent event(0, "Class", Date::Create(2026, 10, 14).value(), "9:05 PM", "10:00 PM", "", "");

    service.CreateEvent(event, "token");

    CHECK(JsonField(http.lastBody, "start", "dateTime").rfind("2026-10-14T21:05:00", 0) == 0);
}

TEST_CASE("GoogleCalendarService CreateEvent moves the end to the next day when it is before the start"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123"})");
    CalendarEvent event(0, "Late night", Date::Create(2026, 12, 31).value(), "23:00", "01:00", "", "");

    service.CreateEvent(event, "token");

    CHECK(JsonField(http.lastBody, "start", "dateTime").rfind("2026-12-31T23:00:00", 0) == 0);
    CHECK(JsonField(http.lastBody, "end", "dateTime").rfind("2027-01-01T01:00:00", 0) == 0);
}

TEST_CASE("GoogleCalendarService CreateEvent does not call the API when a time is invalid"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    CalendarEvent event(0, "Broken", Date::Create(2026, 10, 14).value(), "25:99", "11:00", "", "");

    CalendarEvent created = service.CreateEvent(event, "token");

    CHECK(created.GetGoogleEventId().empty());
    CHECK(http.postCallCount == 0);
}

TEST_CASE("GoogleCalendarService UpdateEvent does not call the API when a time is invalid"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    CalendarEvent event(0, "Broken", Date::Create(2026, 10, 14).value(), "", "", "", "");
    event.SetGoogleEventId("abc123");

    CHECK_FALSE(service.UpdateEvent(event, "token"));
    CHECK(http.patchCallCount == 0);
}

TEST_CASE("GoogleCalendarService GetEvent returns the same date and times that CreateEvent sent"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123"})");
    service.CreateEvent(MakeEvent(), "token");

    nlohmann::json sent = nlohmann::json::parse(http.lastBody);
    sent["id"] = "abc123";
    http.QueueResponse(200, sent.dump());

    std::optional<CalendarEvent> event = service.GetEvent("abc123", "token");

    REQUIRE(event.has_value());
    CHECK(event->GetDate() == Date::Create(2026, 10, 14).value());
    CHECK(event->GetStartTime() == "10:00");
    CHECK(event->GetEndTime() == "12:00");
    CHECK(event->GetLocation() == "Library");
    CHECK(event->GetDescription() == "Review Biology Deck");
    CHECK(event->GetReminderMinutes() == 15);
}

TEST_CASE("GoogleCalendarService GetEvent reads an all-day event as a date with no times"){
    FakeHttpClient http;
    GoogleCalendarService service(http);
    http.QueueResponse(200, R"({"id":"abc123","summary":"Holiday","start":{"date":"2026-12-25"},"end":{"date":"2026-12-26"}})");

    std::optional<CalendarEvent> event = service.GetEvent("abc123", "token");

    REQUIRE(event.has_value());
    CHECK(event->GetDate() == Date::Create(2026, 12, 25).value());
    CHECK(event->GetStartTime() == "");
    CHECK(event->GetEndTime() == "");
}