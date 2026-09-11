#ifndef UPDATE_COURSE_USE_CASE_H
#define UPDATE_COURSE_USE_CASE_H

#include "update_course_request.h"
#include "../../../ServiceInterfaces/course_db_service.h"

class UpdateCourseUseCase {
	public:
		UpdateCourseUseCase(ICourseDBService& courseDBService) : courseDBService(courseDBService) {}

		bool Execute(const UpdateCourseRequest& request);

	private:
		ICourseDBService& courseDBService;
};

#endif
