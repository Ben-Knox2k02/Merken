#ifndef COURSE_DB_SERVICE_DUMMY_H
#define COURSE_DB_SERVICE_DUMMY_H

#include "../../../UseCases/Services/course_db_service.h"

// dummy in-memory placeholder, does not persist to a real database yet
class CourseDbService : public ICourseDBService {
	public:
		std::vector<Course> GetCourses() override;
		std::optional<Course> GetCourse(int courseID) override;
		bool AddCourse(const Course& course) override;
		bool UpdateCourse(const Course& course) override;
		bool DeleteCourse(int courseID) override;

	private:
		std::vector<Course> courses;
};

#endif
