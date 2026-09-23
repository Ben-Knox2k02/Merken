#include "google_calendar_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"

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
	(void)apiKey;
	(void)this->httpClient;
	return {};
}

bool GoogleCalendarService::UpdateEvent(const CalendarEvent& event, const std::string& apiKey) {
	(void)event;
	(void)apiKey;
	(void)this->httpClient;
	return true;
}

