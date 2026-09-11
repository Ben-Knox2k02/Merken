#ifndef COURSE_DB_SERVICE_H
#define COURSE_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../DomainModels/course.h"

class ICourseDBService {
	public:
		virtual ~ICourseDBService() = default;

		virtual std::vector<Course> GetCourses() = 0;
		virtual std::optional<Course> GetCourse(std::string courseID) = 0;
		virtual bool AddCourse(const Course& course) = 0;
		virtual bool UpdateCourse(const Course& course) = 0;
		virtual bool DeleteCourse(std::string courseID) = 0;
};

#endif
