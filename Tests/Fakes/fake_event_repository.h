#ifndef FAKE_EVENT_REPOSITORY_H
#define FAKE_EVENT_REPOSITORY_H

#include <vector>
#include <optional>
#include "../../Application/Repositories/event_repository.h"

class FakeEventRepository : public IEventRepository {
	public:
		int nextId = 1;
		bool addSucceeds = true;
		bool updateSucceeds = true;
		std::vector<CalendarEvent> events;

		int Seed(
			const Date& date,
			const std::string& title,
			const std::string& startTime,
			const std::string& endTime,
			const std::string& location = "",
			const std::string& description = "",
			int reminderMinutes = 60,
			const std::string& googleEventId = ""
		) {
			CalendarEvent event(0, title, date, startTime, endTime, location, description);
			event.UpdateReminderMinutes(reminderMinutes);
			event.SetGoogleEventId(googleEventId);
			return this->AddEvent(event);
		}

		std::vector<CalendarEvent> GetEvents() override {
			return this->events;
		}

		std::vector<CalendarEvent> GetEventsForDate(const Date& date) override {
			std::vector<CalendarEvent> matched;
			for (const CalendarEvent& event : this->events) {
				if (event.GetDate() == date) {
					matched.push_back(event);
				}
			}
			return matched;
		}

		std::optional<CalendarEvent> GetEvent(int calendarEventId) override {
			for (const CalendarEvent& event : this->events) {
				if (event.GetCalendarEventId() == calendarEventId) {
					return event;
				}
			}
			return std::nullopt;
		}

		int AddEvent(const CalendarEvent& event) override {
			if (!this->addSucceeds) {
				return 0;
			}
			const int eventId = this->nextId;
			this->nextId += 1;
			CalendarEvent stored(
				eventId,
				event.GetTitle(),
				event.GetDate(),
				event.GetStartTime(),
				event.GetEndTime(),
				event.GetLocation(),
				event.GetDescription()
			);
			stored.UpdateReminderMinutes(event.GetReminderMinutes());
			stored.SetGoogleEventId(event.GetGoogleEventId());
			this->events.push_back(stored);
			return eventId;
		}

		bool UpdateEvent(const CalendarEvent& event) override {
			if (!this->updateSucceeds) {
				return false;
			}
			for (CalendarEvent& stored : this->events) {
				if (stored.GetCalendarEventId() == event.GetCalendarEventId()) {
					stored = event;
					return true;
				}
			}
			return false;
		}

		bool DeleteEvent(int calendarEventId) override {
			for (auto it = this->events.begin(); it != this->events.end(); ++it) {
				if (it->GetCalendarEventId() == calendarEventId) {
					this->events.erase(it);
					return true;
				}
			}
			return false;
		}
};

#endif
