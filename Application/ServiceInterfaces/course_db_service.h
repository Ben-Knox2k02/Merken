#ifndef COURSE_DB_SERVICE_H
#define COURSE_DB_SERVICE_H

#include <vector>
#include <optional>
#include "../../DomainModels/course.h"

class ICourseDBService {
	public:
		virtual ~ICourseDBService() = default;

		virtual std::vector<Course> GetCourses() = 0;
		virtual std::optional<Course> GetCourse(int courseId) = 0;
		virtual int AddCourse(const Course& course) = 0;
		virtual bool UpdateCourse(const Course& course) = 0;
		virtual bool DeleteCourse(int courseId) = 0;
};

#endif
