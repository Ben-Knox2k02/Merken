#ifndef CREATE_COURSE_USE_CASE_H
#define CREATE_COURSE_USE_CASE_H

#include "create_course_request.h"
#include "create_course_response.h"
#include "../../../ServiceInterfaces/course_db_service.h"

class CreateCourseUseCase {
	public:
		CreateCourseUseCase(ICourseDBService& courseDBService)
			: courseDBService(courseDBService) {}

		CreateCourseResponse Execute(const CreateCourseRequest& request);

	private:
		ICourseDBService& courseDBService;
};

#endif
