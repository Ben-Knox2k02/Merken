#include "create_course_usecase.h"

CreateCourseResponse CreateCourseUseCase::Execute(const CreateCourseRequest& request) {
	std::string newCourseId = this->uuidGeneratorService.GenerateUuid();
	Course course(newCourseId, request.name);
	
	this->courseDBService.AddCourse(course);
	return CreateCourseResponse{course.GetId(), course.GetName()};
}
