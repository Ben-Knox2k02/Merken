#ifndef FAKE_CALENDAR_API_SERVICE_H
#define FAKE_CALENDAR_API_SERVICE_H

#include <string>
#include <vector>
#include <optional>
#include "../../Application/Services/calendar_api_service.h"

class FakeCalendarApiService : public ICalendarAPIService {
	public:
		int nextEventId = 1;
		bool updateEventSucceeds = true;
		std::vector<CalendarEvent> events;
		std::vector<CalendarEvent> createdEvents;
		std::vector<CalendarEvent> updatedEvents;
		std::string lastApiKey;

		CalendarEvent SeedEvent(
			int calendarEventId,
			const std::string& googleEventId,
			const std::string& title,
			const std::string& startTime,
			const std::string& endTime
		) {
			CalendarEvent event(calendarEventId, title, startTime, endTime);
			event.SetGoogleEventId(googleEventId);
			this->events.push_back(event);
			return event;
		}

		CalendarEvent CreateEvent(const CalendarEvent& event, const std::string& apiKey) override {
			this->createdEvents.push_back(event);
			this->lastApiKey = apiKey;

			const int eventId = this->nextEventId;
			this->nextEventId += 1;
			CalendarEvent created(eventId, event.GetTitle(), event.GetStartTime(), event.GetEndTime());
			created.UpdateDescription(event.GetDescription());
			created.UpdateReminderMinutes(event.GetReminderMinutes());
			created.SetGoogleEventId("gcal-" + std::to_string(eventId));
			this->events.push_back(created);
			return created;
		}

		std::optional<CalendarEvent> GetEvent(const std::string& googleEventId, const std::string& apiKey) override {
			this->lastApiKey = apiKey;
			if (googleEventId.empty()) {
				return std::nullopt;
			}
			for (const CalendarEvent& event : this->events) {
				if (event.GetGoogleEventId() == googleEventId) {
					return event;
				}
			}
			return std::nullopt;
		}

		bool UpdateEvent(const CalendarEvent& event, const std::string& apiKey) override {
			this->updatedEvents.push_back(event);
			this->lastApiKey = apiKey;
			if (event.GetGoogleEventId().empty()) {
				return false;
			}
			return this->updateEventSucceeds;
		}
};

#endif
