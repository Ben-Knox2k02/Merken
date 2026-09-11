#ifndef COURSE_DB_SERVICE_REAL_H
#define COURSE_DB_SERVICE_REAL_H

#include "../../../Application/ServiceInterfaces/course_db_service.h"
#include <wx/wxsqlite3.h>
#include <database_context.h>

class CourseDbService : public ICourseDBService {
	public:
		CourseDbService(DatabaseContext& dbContext): db(dbContext) {
			this->EnsureSchema();
		};

		std::vector<Course> GetCourses() override;
		std::optional<Course> GetCourse(std::string courseID) override;
		bool AddCourse(const Course& course) override;
		bool UpdateCourse(const Course& course) override;
		bool DeleteCourse(std::string courseID) override;

	private:
		DatabaseContext& db;
		void EnsureSchema();
};

#endif
