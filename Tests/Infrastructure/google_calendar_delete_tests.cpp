#include "doctest/doctest.h"
#include "../Fakes/fake_http_client.h"
#include "../../Infrastructure/GoogleCalendar/google_calendar_service.h"

TEST_CASE("GoogleCalendarService DeleteEvent sends an authorized DELETE for the event") {
	FakeHttpClient http;
	http.QueueResponse(204, "");
	GoogleCalendarService service(http);

	CHECK(service.DeleteEvent("abc123", "token-1"));

	CHECK(http.deleteCallCount == 1);
	CHECK(http.requestedMethods.back() == "DELETE");
	CHECK(http.lastUrl == "https://www.googleapis.com/calendar/v3/calendars/primary/events/abc123");
	CHECK(http.LastHeaderValue("Authorization") == "Bearer token-1");
}

TEST_CASE("GoogleCalendarService DeleteEvent encodes the event id in the URL") {
	FakeHttpClient http;
	http.QueueResponse(204, "");
	GoogleCalendarService service(http);

	CHECK(service.DeleteEvent("a/b", "token-1"));
	CHECK(http.lastUrl == "https://www.googleapis.com/calendar/v3/calendars/primary/events/a%2Fb");
}

TEST_CASE("GoogleCalendarService DeleteEvent treats an already-deleted event as success") {
	FakeHttpClient http;
	http.QueueResponse(404, R"({"error":{"message":"Not Found"}})");
	http.QueueResponse(410, R"({"error":{"message":"Resource has been deleted"}})");
	GoogleCalendarService service(http);

	CHECK(service.DeleteEvent("abc123", "token-1"));
	CHECK(service.DeleteEvent("abc123", "token-1"));
}

TEST_CASE("GoogleCalendarService DeleteEvent fails when Google rejects the request") {
	FakeHttpClient http;
	http.QueueResponse(401, R"({"error":{"message":"Invalid Credentials"}})");
	http.QueueResponse(500, R"({"error":{"message":"Backend Error"}})");
	GoogleCalendarService service(http);

	CHECK_FALSE(service.DeleteEvent("abc123", "token-1"));
	CHECK_FALSE(service.DeleteEvent("abc123", "token-1"));
}

TEST_CASE("GoogleCalendarService DeleteEvent fails on a network error") {
	FakeHttpClient http;
	http.QueueResponse(0, "", "timeout");
	GoogleCalendarService service(http);

	CHECK_FALSE(service.DeleteEvent("abc123", "token-1"));
}

TEST_CASE("GoogleCalendarService DeleteEvent makes no request without a token or event id") {
	FakeHttpClient http;
	GoogleCalendarService service(http);

	CHECK_FALSE(service.DeleteEvent("abc123", ""));
	CHECK_FALSE(service.DeleteEvent("", "token-1"));
	CHECK(http.deleteCallCount == 0);
}