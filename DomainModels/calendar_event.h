#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include <string>

class CalendarEvent {
	public:
		CalendarEvent(
			int calendarEventId,
			const std::string& title,
			const std::string& date,
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
		const std::string& GetGoogleEventId() const { return this->googleEventId; }
		const std::string& GetTitle() const { return this->title; }
		const std::string& GetDate() const { return this->date; }
		const std::string& GetStartTime() const { return this->startTime; }
		const std::string& GetEndTime() const { return this->endTime; }
		const std::string& GetLocation() const { return this->location; }
		const std::string& GetDescription() const { return this->description; }
		int GetReminderMinutes() const { return this->reminderMinutes; }
		

		void UpdateTitle(const std::string& newTitle) {
			this->title = newTitle;
		}
		
		void UpdateDate(const std::string& newDate) {
			this->date = newDate;
		}
		
		void UpdateTimeRange(const std::string& newStartTime, const std::string& newEndTime) {
			this->startTime = newStartTime;
			this->endTime = newEndTime;
		}
		
		void UpdateLocation(const std::string& newLocation) {
			this->location = newLocation;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
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
		std::string date;
		std::string startTime;
		std::string endTime;
		std::string location;
		std::string description;
		int reminderMinutes;
		std::string googleEventId;
		
};

#endif
