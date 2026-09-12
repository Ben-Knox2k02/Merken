#ifndef FAKE_CALENDAR_API_SERVICE_H
#define FAKE_CALENDAR_API_SERVICE_H

#include <string>
#include <vector>
#include "../../Application/ServiceInterfaces/calendar_api_service.h"

class FakeCalendarApiService : public ICalendarAPIService {
	public:
		int nextEventId = 1;
		std::vector<CalendarEvent> createdEvents;
		AppSettings lastSettings;

		CalendarEvent CreateEvent(const CalendarEvent& event, const AppSettings& settings) override {
			this->createdEvents.push_back(event);
			this->lastSettings = settings;

			const int eventId = this->nextEventId;
			this->nextEventId += 1;
			CalendarEvent created(eventId, event.GetTitle(), event.GetStartTime(), event.GetEndTime());
			created.UpdateDescription(event.GetDescription());
			created.UpdateReminderMinutes(event.GetReminderMinutes());
			created.SetGoogleEventId("gcal-" + std::to_string(eventId));
			return created;
		}

		bool UpdateEvent(const CalendarEvent&, const AppSettings&) override {
			return false;
		}
};

#endif
