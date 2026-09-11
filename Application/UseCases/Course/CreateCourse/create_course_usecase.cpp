#include "create_course_usecase.h"

CreateCourseResponse CreateCourseUseCase::Execute(const CreateCourseRequest& request) {
	Course course(0, request.name);
	int courseId = this->courseDBService.AddCourse(course);
	return CreateCourseResponse{courseId, request.name};
}
