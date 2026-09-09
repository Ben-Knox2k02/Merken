#include "update_course_use_case.h"

bool UpdateCourseUseCase::Execute(const UpdateCourseRequest& request) {
	std::optional<Course> existingCourse = this->courseDBService.GetCourse(request.courseID);
	if(!existingCourse.has_value()) { return false; }

	existingCourse->UpdateName(request.name);
	return this->courseDBService.UpdateCourse(*existingCourse);
}
