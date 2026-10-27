#include "doctest/doctest.h"
#include <string>
#include "../Fakes/fake_http_client.h"
#include "../../Infrastructure/GoogleCalendar/google_calendar_service.h"

static const std::string kBaseUrl = "https://www.googleapis.com/calendar/v3/calendars/primary/events";

static CalendarEvent MakeEvent(){
    CalendarEvent event(0, "Study Session", "2026-10-14T10:00:00-05:00", "2026-10-14T12:00:00-05:00");
    event.UpdateDescription("Review biology deck");
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