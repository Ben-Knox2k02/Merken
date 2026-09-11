#ifndef CREATE_COURSE_USE_CASE_H
#define CREATE_COURSE_USE_CASE_H

#include "create_course_request.h"
#include "create_course_response.h"
#include "../../../ServiceInterfaces/course_db_service.h"
#include "../../../ServiceInterfaces/uuid_generator_service.h"

class CreateCourseUseCase {
	public:
		CreateCourseUseCase(
			ICourseDBService& courseDBService,
			IUuidGeneratorService& uuidGeneratorService
		) : courseDBService(courseDBService),
			uuidGeneratorService(uuidGeneratorService) {}

		CreateCourseResponse Execute(const CreateCourseRequest& request);

	private:
		ICourseDBService& courseDBService;
		IUuidGeneratorService& uuidGeneratorService;
};

#endif
