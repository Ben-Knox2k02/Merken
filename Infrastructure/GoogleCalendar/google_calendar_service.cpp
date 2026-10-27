#include "google_calendar_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>


static const std::string kEventsUrl = "https://www.googleapis.com/calendar/v3/calendars/primary/events";
static const int kMaxPages = 50; //prevents a potential infinite loop
static void LogCalendarError(const std::string& operation, int statusCode, const std::string& detail){
	std::cerr << "[GoogleCalendar] " << operation << " failed with status code " << statusCode << ": " << detail << std::endl;
}
//reading and writing to a user's calendar needs an OAuth access token as a bearer header
static std::vector<HttpHeader> AuthHeaders(const std::string& accessToken) {
	return { { "Authorization", "Bearer " + accessToken },
			 { "Content-Type", "application/json" } };
}

static bool IsSuccessStatus(int statusCode) {
	return statusCode >= 200 && statusCode < 300;
}

//percent-encodes a string for use in a URL
static std::string UrlEncode(const std::string& value) {
	std::ostringstream escaped;
	escaped << std::hex << std::uppercase;
	for (unsigned char c : value){
		if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
		} else {
			escaped << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
		}
	}
	return escaped.str();
}

//reads a string field from a JSON object and returns "" if it is missing or not a string
static std::string GetStringField(const nlohmann::json& obj, const char* key){
	if(obj.is_object() && obj.contains(key) && obj[key].is_string()){
		return obj[key].get<std::string>();
	}
	return "";
}

static std::string GetEventTime(const nlohmann::json& json, const char* key){
	if(!json.contains(key) || !json[key].is_object()){
		return "";
	}
	std::string dateTime = GetStringField(json[key], "dateTime");
	return dateTime.empty() ? GetStringField(json[key], "date") : dateTime;
}

static std::string ExtractGoogleErrorMessage(const std::string& body){
	nlohmann::json j = nlohmann::json::parse(body, nullptr, false);
	if(j.is_object() && j.contains("error") && j["error"].is_object()){
		return j["error"].value("message", "Unknown error");
	}
	return body.substr(0, 200);
}

//describe why a request failed
static std::string DescribeFailure(const HttpResponse& response){
	if(!response.error.empty()){
		return response.error;
	}
	return ExtractGoogleErrorMessage(response.body);
}

static nlohmann::json BuildEventPayload(const CalendarEvent& event){
	nlohmann::json payload;
	payload["summary"] = event.GetTitle();
	payload["description"] = event.GetDescription();
	payload["start"]["dateTime"] = event.GetStartTime();
	payload["end"]["dateTime"] = event.GetEndTime();
	payload["reminders"]["useDefault"] = false;
	payload["reminders"]["overrides"] = nlohmann::json::array(
		{{{"method", "popup"},{"minutes", event.GetReminderMinutes()}}});
	return payload;
}

// Helper function to parse a Google Calendar event JSON object into a CalendarEvent instance (created with help from Gemini)
CalendarEvent ParseGoogleEventJson(const nlohmann::json& json){
	if(!json.is_object()){
		return CalendarEvent(0, "", "", "");
	}
	
	std::string summary = GetStringField(json, "summary");
	std::string description = GetStringField(json, "description");

	std::string startTime = GetEventTime(json, "start");
	std::string endTime = GetEventTime(json, "end");
	int reminderMinutes = 0;
	if (json.contains("reminders") && json["reminders"].is_object() && json["reminders"].contains("overrides")
		&& json["reminders"]["overrides"].is_array()) {
		for (const auto& reminder : json["reminders"]["overrides"]) {
			if (reminder.value("method", "") == "popup") {
				reminderMinutes = reminder.value("minutes", 0);
				break;
			}
		}
	}

	CalendarEvent event(0, summary, startTime, endTime);
	if (!description.empty()) {
		event.UpdateDescription(description);
	}
	event.UpdateReminderMinutes(reminderMinutes);

	std::string id = GetStringField(json, "id");
	if (!id.empty()) {
		event.SetGoogleEventId(id);
	}
	return event;
}

// Calls calendar API to create a new event and returns the created event with the Google Event ID set. If the API call fails, it returns the original event without any modifications.
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const std::string& accessToken) {
	if (accessToken.empty()) {
		LogCalendarError("CreateEvent", 0, "No access token provided");
		return event;
	}
	
	try {
		


		HttpResponse response = this->httpClient.PostJson(kEventsUrl, BuildEventPayload(event).dump(), AuthHeaders(accessToken));

		if (!IsSuccessStatus(response.statusCode)){
			LogCalendarError("CreateEvent", response.statusCode, DescribeFailure(response));
			return event;
		}
		nlohmann::json jsonResponse = nlohmann::json::parse(response.body, nullptr, false);
		std::string id = GetStringField(jsonResponse, "id");
		if (id.empty()){
			LogCalendarError("CreateEvent", response.statusCode, "Failed to retrieve event ID from response");
			return event;
		}
		CalendarEvent createdEvent = event;
		createdEvent.SetGoogleEventId(id);
		return createdEvent;
	} catch (const std::exception& e) {
		LogCalendarError("CreateEvent", 0, e.what());
	}
	return event;
	
}

std::optional<CalendarEvent> GoogleCalendarService::GetEvent(const std::string& googleEventId, const std::string& accessToken) {
	if (googleEventId.empty() || accessToken.empty()) {
		LogCalendarError("GetEvent", 0, "No Google Event ID or access token provided");
		return std::nullopt;
	}
	
	try {
		std::string url = kEventsUrl + "/" + UrlEncode(googleEventId);

		HttpResponse response = this->httpClient.Get(url, AuthHeaders(accessToken));

		if (!IsSuccessStatus(response.statusCode) || response.body.empty()){
			LogCalendarError("GetEvent", response.statusCode, DescribeFailure(response));
			return std::nullopt;
		}
		nlohmann::json jsonResponse = nlohmann::json::parse(response.body, nullptr, false);
		if(!jsonResponse.is_object()){
			LogCalendarError("GetEvent", response.statusCode, "Response was not valid JSON");
			return std::nullopt;
		}
		return ParseGoogleEventJson(jsonResponse);
	} catch (const std::exception& e) {
		LogCalendarError("GetEvent", 0, e.what());
	}
	return std::nullopt;
}

std::vector<CalendarEvent> GoogleCalendarService::GetEvents(const std::string& accessToken) {
	std::vector<CalendarEvent> events;
	if (accessToken.empty()) {
		LogCalendarError("GetEvents", 0, "No access token provided");
		return events;
	}

	std::vector<HttpHeader> headers = AuthHeaders(accessToken);
	std::string pageToken;

	try {
		int pagesFetched = 0;
		do {
			std::string url = kEventsUrl;
			if (!pageToken.empty()) {
				url += "?pageToken=" + UrlEncode(pageToken);
			}
			HttpResponse response = this->httpClient.Get(url, headers);

			if(!IsSuccessStatus(response.statusCode) || response.body.empty()){
				LogCalendarError("GetEvents", response.statusCode, DescribeFailure(response));
				break;
			}

			nlohmann::json jsonResponse = nlohmann::json::parse(response.body, nullptr, false);
			if(!jsonResponse.is_object()){
				LogCalendarError("GetEvents", response.statusCode, "Response was not valid JSON");
				break;
			}

			if (jsonResponse.contains("items") && jsonResponse["items"].is_array()){
				for (const auto& item : jsonResponse["items"]){
					if (item.value("status", "") == "cancelled"){
						continue;
					}
					events.push_back(ParseGoogleEventJson(item));
				}
			}

			pageToken = jsonResponse.value("nextPageToken", "");
			++pagesFetched;
			
		} while (!pageToken.empty() && pagesFetched < kMaxPages);
		if(pagesFetched >= kMaxPages && !pageToken.empty()){
			LogCalendarError("GetEvents", 0, "Stopped after reaching page limit. Some events may be missing");
		}
	} catch (const std::exception& e) {
		LogCalendarError("GetEvents", 0, e.what());
	}
	return events;
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const std::string& accessToken) {
	if (accessToken.empty()){
		LogCalendarError("UpdateEvent", 0, "No access token provided");
		return false;
	}
	if (event.GetGoogleEventId().empty()){
		LogCalendarError("UpdateEvent", 0, "No Google event ID provided");
		return false;
	}
	try {
		std::string url = kEventsUrl + "/" + UrlEncode(event.GetGoogleEventId());
		HttpResponse response = this->httpClient.PatchJson(url, BuildEventPayload(event).dump(), AuthHeaders(accessToken));
		if(!IsSuccessStatus(response.statusCode)){
			LogCalendarError("UpdateEvent", response.statusCode, DescribeFailure(response));
			return false;
		}
		return true;
	} catch (const std::exception& e){
		LogCalendarError("UpdateEvent", 0, e.what());
	}
	return false;

}

