#include "course_db_service.h"
#include <algorithm>

std::vector<Course> CourseDbService::GetCourses() {
	return this->courses;
}

std::optional<Course> CourseDbService::GetCourse(int courseID) {
	for(const auto& course : this->courses) {
		if(course.GetId() == courseID) { return course; }
	}
	return std::nullopt;
}

bool CourseDbService::AddCourse(const Course& course) {
	this->courses.push_back(course);
	return true;
}

bool CourseDbService::UpdateCourse(const Course& course) {
	for(auto& existing : this->courses) {
		if(existing.GetId() == course.GetId()) {
			existing.UpdateName(course.GetName());
			return true;
		}
	}
	return false;
}

bool CourseDbService::DeleteCourse(int courseID) {
	auto it = std::remove_if(this->courses.begin(), this->courses.end(), [courseID](const Course& course) {
		return course.GetId() == courseID;
	});

	if(it == this->courses.end()) { return false; }

	this->courses.erase(it, this->courses.end());
	return true;
}
