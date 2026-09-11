#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H

#include <string>
#include <wx/datetime.h>

class CalendarEvent {
	public:
		CalendarEvent(
			std::string id,
			std::string courseID,
			const std::string& title,
			const std::string& description,
			const wxDateTime& startTime,
			const wxDateTime& endTime
		) : id(id),
			courseID(courseID),
			title(title),
			description(description),
			startTime(startTime),
			endTime(endTime) {}

		const std::string& GetId() const { return this->id; }
		const std::string& GetCourseID() const { return this->courseID; }
		const std::string& GetTitle() const { return this->title; }
		const std::string& GetDescription() const { return this->description; }
		const wxDateTime& GetStartTime() const { return this->startTime; }
		const wxDateTime& GetEndTime() const { return this->endTime; }

		void UpdateTimeRange(const wxDateTime& newStartTime, const wxDateTime& newEndTime) {
			this->startTime = newStartTime;
			this->endTime = newEndTime;
		}

		void UpdateTitle(const std::string& newTitle) {
			this->title = newTitle;
		}

		void UpdateDescription(const std::string& newDescription) {
			this->description = newDescription;
		}

	private:
		std::string id;
		std::string courseID;
		std::string title;
		std::string description;
		wxDateTime startTime;
		wxDateTime endTime;
};

#endif
