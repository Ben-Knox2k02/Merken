#ifndef COURSE_H
#define COURSE_H

#include <string>

// Leftover from the CourseTracker naming. SRS entity is Deck, not Course.
class Course {
	public:
		Course(int courseId, const std::string& name) : courseId(courseId), name(name) {}

		int GetCourseId() const { return this->courseId; }
		const std::string& GetName() const { return this->name; }

		void UpdateName(const std::string& newName) {
			this->name = newName;
		}

	private:
		int courseId;
		std::string name;
};

#endif
