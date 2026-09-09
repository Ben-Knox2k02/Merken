#include "create_course_use_case.h"

Course CreateCourseUseCase::Execute(const CreateCourseRequest& request) {
	Course course(0, request.name);
	this->courseDBService.AddCourse(course);
	return course;
}
