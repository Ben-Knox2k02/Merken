#include "google_calendar_service.h"
#include "../../ThirdParty/nlohmann/json.hpp"
#include <cctype>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
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

// days since 1970-01-01 for a calendar date
static long long DaysFromCivil(int year, int month, int day){
	year -= month <= 2 ? 1 : 0;
	const long long era = (year >= 0 ? year : year - 399) / 400;
	const long long yearOfEra = year - era * 400;
	const long long dayOfYear = (153LL * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
	const long long dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
	return era * 146097 + dayOfEra - 719468;
}

//treat a field of a tm as if it were UTC
static long long TmAsUtcSeconds(const std::tm& t){
	return DaysFromCivil(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday) * 86400LL + t.tm_hour * 3600LL + t.tm_min * 60LL + t.tm_sec;
}

static long long LocalUtcOffsetSeconds(const Date& date, int hour, int minute){
	std::tm local{};
	local.tm_year = date.GetYear() - 1900;
	local.tm_mon = date.GetMonth() - 1;
	local.tm_mday = date.GetDay();
	local.tm_hour = hour;
	local.tm_min = minute;
	local.tm_sec = 0;
	local.tm_isdst = -1;
	const std::time_t instant = std::mktime(&local);
	if(instant == static_cast<std::time_t>(-1)){
		return 0;
	}
	//localtime and gmtime share one buffer, so we must copy the result to avoid overwriting it
	const std::tm asLocal = *std::localtime(&instant);
	const std::tm asUtc = *std::gmtime(&instant);
	return TmAsUtcSeconds(asLocal) - TmAsUtcSeconds(asUtc);
}

static bool ParseClockTime(const std::string& text, int& hour, int& minute){
	std::string value;
	for (char c : text){
		if (!std::isspace(static_cast<unsigned char>(c))){
			value.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
		}
	}
	bool isPm = false;
	bool hasMerdiem = false;
	if (value.size() > 2 && (value.compare(value.size() -2, 2, "AM") == 0 || value.compare(value.size() -2,2,"PM") == 0)){
		hasMerdiem = true;
		isPm = value[value.size() - 2] == 'P';
		value.resize(value.size() - 2);
	}
	int h = 0;
	int m = 0;
	int s = 0;
	char extra = 0;
	const int matched = std::sscanf(value.c_str(), "%d:%d:%d%c", &h, &m, &s, &extra);
	int fields = matched;
	if (fields < 3){
		s = 0;
		fields = std::sscanf(value.c_str(), "%d:%d%c", &h, &m, &extra);
		if (fields != 2){
			return false;
		}
	}else if( fields != 3){
		return false;
	}
	if (hasMerdiem){
		if (h < 1 || h > 12) { return false;}
		h = (h % 12) + (isPm ? 12 : 0);
	}
	if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
		return false;
	}
	hour = h;
	minute = m;
	return true;
}

static std::string BuildDateTime(const Date& date, int hour, int minute){
	const long long offset = LocalUtcOffsetSeconds(date, hour, minute);
	const long long absoluteOffset = offset < 0 ? -offset : offset;
	char buffer[40] = {0};
	std::snprintf(buffer, sizeof(buffer), "%sT%02d:%02d:00%c%02d:%02d", date.ToIso().c_str(), hour, minute, offset < 0 ? '-' : '+',
	static_cast<int>(absoluteOffset / 3600), static_cast<int>(absoluteOffset % 3600 / 60));
	return buffer;
}

static bool ParseGoogleDateTime(const std::string& text, Date& date, std::string& clock){
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	if (std::sscanf(text.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second) != 6) {
		return false;
	}
	long long offsetSeconds = 0;
	const size_t zone = text.find_first_of("Zz+-", 19);
	if (zone != std::string::npos && text[zone] != 'Z' && text[zone] != 'z') {
		int offsetHours = 0, offsetMinutes = 0;
		if(std::sscanf(text.c_str() + zone + 1, "%d:%d", &offsetHours, &offsetMinutes) == 2) {
			offsetSeconds = (offsetHours * 3600LL + offsetMinutes * 60LL) * (text[zone] == '-' ? -1 : 1);
		}
	}
	if (!Date::IsValid(year, month, day)) {
		return false;
	}
	const long long utcSeconds = DaysFromCivil(year, month, day) * 86400LL + hour * 3600LL + minute * 60LL + second - offsetSeconds;
	const std::time_t instant = static_cast<std::time_t>(utcSeconds);
	const std::tm* local = std::localtime(&instant);
	if (local == nullptr){
		return false;
	}
	std::optional<Date> localDate = Date::Create(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
	if (!localDate.has_value()){
		return false;
	}
	char buffer[8] = {0};
	std::snprintf(buffer, sizeof(buffer), "%02d:%02d", local->tm_hour, local->tm_min);
	date = *localDate;
	clock = buffer;
	return true;
}

static Date FallbackDate() {
	return Date::Create(1970, 1, 1).value();
}

static std::optional<nlohmann::json> BuildEventPayload(const CalendarEvent& event) {
	int startHour = 0, startMinute = 0, endHour = 0, endMinute = 0;
	if (!ParseClockTime(event.GetStartTime(), startHour, startMinute) || !ParseClockTime(event.GetEndTime(), endHour, endMinute)){
		return std::nullopt;
	}

	Date endDate = event.GetDate();
	if (endHour * 60 + endMinute < startHour * 60 + startMinute) {
		endDate = endDate.AddDays(1);
	}

	nlohmann::json payload;
	payload["summary"] = event.GetTitle();
	payload["description"] = event.GetDescription();
	payload["location"] = event.GetLocation();
	payload["start"]["dateTime"] = BuildDateTime(event.GetDate(), startHour, startMinute);
	payload["end"]["dateTime"] = BuildDateTime(endDate, endHour, endMinute);
	payload["reminders"]["useDefault"] = false;
	payload["reminders"]["overrides"] = nlohmann::json::array({{{"method", "popup"}, {"minutes", event.GetReminderMinutes()}}});
	return payload;
	
}

// Helper function to parse a Google Calendar event JSON object into a CalendarEvent instance (created with help from Gemini)
CalendarEvent ParseGoogleEventJson(const nlohmann::json& json){
	if(!json.is_object()){
		return CalendarEvent(0, "", FallbackDate(), "", "", "", "");
	}
	
	std::string summary = GetStringField(json, "summary");
	std::string description = GetStringField(json, "description");
	std::string location = GetStringField(json, "location");
	
	Date date = FallbackDate();
	std::string startTime;
	std::string endTime;
	if (json.contains("start") && json["start"].is_object()) {
		const std::string startDateTime = GetStringField(json["start"], "dateTime");
		if (!startDateTime.empty()) {
			ParseGoogleDateTime(startDateTime, date, startTime);
		}else{
			std::optional<Date> allDay = Date::Parse(GetStringField(json["start"], "date"));
			if (allDay.has_value()) {
				date = *allDay;
			}
		}
	}

	if (json.contains("end") && json["end"].is_object()) {
		Date endDate = date;
		ParseGoogleDateTime(GetStringField(json["end"], "dateTime"), endDate, endTime);
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

	CalendarEvent event(0, summary, date, startTime, endTime, location, description);
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
		std::optional<nlohmann::json> payload = BuildEventPayload(event);
		if (!payload.has_value()) {
			LogCalendarError("CreateEvent", 0, "Start or end time is not a valid time");
			return event;
		}


		HttpResponse response = this->httpClient.PostJson(kEventsUrl, payload->dump(), AuthHeaders(accessToken));

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
		std::optional<nlohmann::json> payload = BuildEventPayload(event);
		if (!payload.has_value()) {
			LogCalendarError("UpdateEvent", 0, "Start or end time is not a valid time");
			return false;
		}
		std::string url = kEventsUrl + "/" + UrlEncode(event.GetGoogleEventId());
		HttpResponse response = this->httpClient.PatchJson(url, payload->dump(), AuthHeaders(accessToken));
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
