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

// Helper function to parse a Google Calendar event JSON object into a CalendarEvent instance (created with help from Gemini)
CalendarEvent ParseGoogleEventJson(const nlohmann::json& json){
	if(!json.is_object()){
		return CalendarEvent(0, "", "", "");
	}
	
	std::string summary = (json.contains("summary") && json["summary"].is_string()) ? json["summary"].get<std::string>() : "";
	std::string description = (json.contains("description") && json["description"].is_string()) ? json["description"].get<std::string>() : "";

	std::string startTime = "";
	if (json.contains("start") && json["start"].is_object() &&
		json["start"].contains("dateTime") && json["start"]["dateTime"].is_string()){
			startTime = json["start"]["dateTime"].get<std::string>();
		}

	std::string endTime = "";
	if (json.contains("end") && json["end"].is_object() &&
		json["end"].contains("dateTime") && json["end"]["dateTime"].is_string()){
			endTime = json["end"]["dateTime"].get<std::string>();
		}
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
	if (json.contains("id") && json["id"].is_string()) {
		event.SetGoogleEventId(json["id"].get<std::string>());
	}
	return event;
}

// Calls calendar API to create a new event and returns the created event with the Google Event ID set. If the API call fails, it returns the original event without any modifications.
CalendarEvent GoogleCalendarService::CreateEvent(const CalendarEvent& event, const std::string& apiKey) {
	if (apiKey.empty()) {
		return event;
	}
	
	try {
		nlohmann::json payload;
		payload["summary"] = event.GetTitle();
		payload["description"] = event.GetDescription();
		payload["start"]["dateTime"] = event.GetStartTime();
		payload["end"]["dateTime"] = event.GetEndTime();
		payload["reminders"]["useDefault"] = false;
		payload["reminders"]["overrides"] = nlohmann::json::array(
			{{{"method", "popup"},{"minutes", event.GetReminderMinutes()}}});

		std::string url = "https://www.googleapis.com/calendar/v3/calendars/primary/events?key=" + apiKey;

		HttpResponse response = this->httpClient.PostJson(url, payload.dump());

		if ((response.statusCode == 200 || response.statusCode == 201) && !response.body.empty()) {
			nlohmann::json jsonResponse = nlohmann::json::parse(response.body);
			if (jsonResponse.contains("id") && jsonResponse["id"].is_string()) {
				CalendarEvent createdEvent = event;
				createdEvent.SetGoogleEventId(jsonResponse["id"].get<std::string>());
				return createdEvent;
			}
		}
	} catch (const std::exception& e) {
		//log the exception if needed
		(void)e; //suppress unused variable warning

	}
	return event;
	
}

std::optional<CalendarEvent> GoogleCalendarService::GetEvent(const std::string& googleEventId, const std::string& apiKey) {
	if (googleEventId.empty() || apiKey.empty()) {
		return std::nullopt;
	}
	
	try {
		std::string url = "https://www.googleapis.com/calendar/v3/calendars/primary/events/" + googleEventId + "?key=" + apiKey;

		HttpResponse response = this->httpClient.Get(url);

		if (response.statusCode == 200 && !response.body.empty()) {
			nlohmann::json jsonResponse = nlohmann::json::parse(response.body);
			CalendarEvent event = ParseGoogleEventJson(jsonResponse);
			return event;
		}
	} catch (const std::exception& e) {
		//log the exception if needed
		(void)e; //suppress unused variable warning
	}
	return std::nullopt;
}

std::vector<CalendarEvent> GoogleCalendarService::GetEvents(const std::string& apiKey) {
	std::vector<CalendarEvent> events;
	if (apiKey.empty()) {
		return events;
	}

	std::vector<HttpHeader> headers = { { "Authorization", "Bearer " + apiKey } };
	std::string pageToken;

	try {
		do {
			std::string url = "https://www.googleapis.com/calendar/v3/calendars/primary/events";
			if (!pageToken.empty()) {
				url += "?pageToken=" + pageToken;
			}
			HttpResponse response = this->httpClient.Get(url, headers);

			if (!response.Ok() || response.body.empty()) {
				break;
			}

			nlohmann::json jsonResponse = nlohmann::json::parse(response.body);

			if (jsonResponse.contains("items") && jsonResponse["items"].is_array()){
				for (const auto& item : jsonResponse["items"]){
					if (item.value("status", "") == "cancelled"){
						continue;
					}
					events.push_back(ParseGoogleEventJson(item));
				}
			}

			pageToken = jsonResponse.value("nextPageToken", "");
		} while (!pageToken.empty());
	} catch (const std::exception& e) {
		//log the exception if needed
		(void)e; //suppress unused variable warning
	}
	return events;
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const std::string& apiKey) {
	(void)event;
	(void)apiKey;
	(void)this->httpClient;
	return false;
}

