#include "course_db_service.h"
#include <wx/wxsqlite3.h>

// Ensures the database schema for the courses table exists.
void CourseDbService::EnsureSchema() {
	this->db.GetConnection()->ExecuteUpdate(
		"CREATE TABLE IF NOT EXISTS courses ("
		"course_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL"
		");"
	);
}

std::vector<Course> CourseDbService::GetCourses() {
	std::vector<Course> courses;

	wxSQLite3::ResultSet result = this
		->db.GetConnection()
		->ExecuteQuery("SELECT course_id, name FROM courses ORDER BY course_id;");

	while(result.NextRow()) {
		courses.emplace_back(result.GetInt(0), result.GetString(1).ToStdString());
	}

	return courses;
}

std::optional<Course> CourseDbService::GetCourse(int courseId) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("SELECT course_id, name FROM courses WHERE course_id = ?;");

	stmt.Bind(1, courseId);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if(!result.NextRow()) { return std::nullopt; }

	return Course(result.GetInt(0), result.GetString(1).ToStdString());
}

int CourseDbService::AddCourse(const Course& course) {
	try {
		wxSQLite3::Statement stmt = this
			->db.GetConnection()
			->PrepareStatement("INSERT INTO courses (name) VALUES (?);");
		stmt.Bind(1, wxString(course.GetName()));
		if (stmt.ExecuteUpdate() <= 0) { return 0; }
		return static_cast<int>(this->db.GetConnection()->GetLastRowId().GetValue());
	} catch(const wxSQLite3::Exception&) {
		return 0;
	}
}

bool CourseDbService::UpdateCourse(const Course& course) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("UPDATE courses SET name = ? WHERE course_id = ?;");

	stmt.Bind(1, wxString(course.GetName()));
	stmt.Bind(2, course.GetCourseId());
	return stmt.ExecuteUpdate() > 0;
}

bool CourseDbService::DeleteCourse(int courseId) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("DELETE FROM courses WHERE course_id = ?;");

	stmt.Bind(1, courseId);
	return stmt.ExecuteUpdate() > 0;
}
