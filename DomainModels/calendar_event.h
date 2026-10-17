#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include <string>

class CalendarEvent {
	public:
		CalendarEvent(
			int calendarEventId,
			const std::string& title,
			const std::string& startTime,
			const std::string& endTime
		) : calendarEventId(calendarEventId),
			title(title),
			startTime(startTime),
			endTime(endTime),
			reminderMinutes(60) {}

		int GetCalendarEventId() const { return this->calendarEventId; }
		const std::string& GetTitle() const { return this->title; }
		const std::string& GetDescription() const { return this->description; }
		const std::string& GetStartTime() const { return this->startTime; }
		const std::string& GetEndTime() const { return this->endTime; }
		int GetReminderMinutes() const { return this->reminderMinutes; }
		const std::string& GetGoogleEventId() const { return this->googleEventId; }

		void UpdateTitle(const std::string& newTitle) {
			this->title = newTitle;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

		void UpdateTimeRange(const std::string& newStartTime, const std::string& newEndTime) {
			this->startTime = newStartTime;
			this->endTime = newEndTime;
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
		std::string description;
		std::string startTime;
		std::string endTime;
		int reminderMinutes;
		std::string googleEventId;
};

#endif
