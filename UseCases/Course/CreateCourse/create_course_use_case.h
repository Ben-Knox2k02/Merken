#ifndef CREATE_COURSE_USE_CASE_H
#define CREATE_COURSE_USE_CASE_H

#include "create_course_request.h"
#include "../../Services/course_db_service.h"

class CreateCourseUseCase {
	public:
		CreateCourseUseCase(ICourseDBService& courseDBService) : courseDBService(courseDBService) {}

		Course Execute(const CreateCourseRequest& request);

	private:
		ICourseDBService& courseDBService;
};

#endif
