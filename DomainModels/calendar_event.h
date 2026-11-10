#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include "date.h"
#include <string>

class CalendarEvent {
	public:
		CalendarEvent(
			int calendarEventId,
			const std::string& title,
			const std::string& startTime,
			const std::string& endTime
		) : CalendarEvent(
				calendarEventId,
				title,
				Date::Create(1970, 1, 1).value(),
				startTime,
				endTime,
				"",
				""
			) {}

		CalendarEvent(
			int calendarEventId,
			const std::string& title,
			const Date& date,
			const std::string& startTime,
			const std::string& endTime,
			const std::string& location,
			const std::string& description
		) : calendarEventId(calendarEventId),
			title(title),
			date(date),
			startTime(startTime),
			endTime(endTime),
			location(location),
			description(description),
			reminderMinutes(60) {}

		int GetCalendarEventId() const { return this->calendarEventId; }
		const std::string& GetTitle() const { return this->title; }
		const Date& GetDate() const { return this->date; }
		const std::string& GetDescription() const { return this->description; }
		const std::string& GetStartTime() const { return this->startTime; }
		const std::string& GetEndTime() const { return this->endTime; }
		const std::string& GetLocation() const { return this->location; }
		int GetReminderMinutes() const { return this->reminderMinutes; }
		const std::string& GetGoogleEventId() const { return this->googleEventId; }

		void UpdateTitle(const std::string& newTitle) {
			this->title = newTitle;
		}

		void UpdateDate(const Date& newDate) {
			this->date = newDate;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

		void UpdateTimeRange(const std::string& newStartTime, const std::string& newEndTime) {
			this->startTime = newStartTime;
			this->endTime = newEndTime;
		}

		void UpdateLocation(const std::string& newLocation) {
			this->location = newLocation;
		}

		void UpdateReminderMinutes(int minutes) {
			this->reminderMinutes = minutes;
		}

		void SetGoogleEventId(const std::string& eventId) {
			this->googleEventId = eventId;
		}

	private:
		int calendarEventId;
		std::string title;
		Date date;
		std::string description;
		std::string startTime;
		std::string endTime;
		std::string location;
		int reminderMinutes;
		std::string googleEventId;
};

#endif
