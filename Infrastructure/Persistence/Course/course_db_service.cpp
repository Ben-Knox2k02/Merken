#include "course_db_service.h"

// Ensures the database schema for the courses table exists.
void CourseDbService::EnsureSchema() {
	this->db.GetConnection()->ExecuteUpdate(
		"CREATE TABLE IF NOT EXISTS courses ("
		"id TEXT PRIMARY KEY,"
		"name TEXT NOT NULL"
		");"
	);
}

std::vector<Course> CourseDbService::GetCourses() {
	std::vector<Course> courses;

	wxSQLite3::ResultSet result = this
		->db.GetConnection()
		->ExecuteQuery("SELECT id, name FROM courses ORDER BY id;");

	while(result.NextRow()) {
		courses.emplace_back(result.GetString(0).ToStdString(), result.GetString(1).ToStdString());
	}

	return courses;
}

std::optional<Course> CourseDbService::GetCourse(std::string courseID) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("SELECT id, name FROM courses WHERE id = ?;");

	stmt.Bind(1, courseID);

	wxSQLite3::ResultSet result = stmt.ExecuteQuery();
	if(!result.NextRow()) { return std::nullopt; }

	return Course(result.GetString(0).ToStdString(), result.GetString(1).ToStdString());
}

bool CourseDbService::AddCourse(const Course& course) {
	try {
		wxSQLite3::Statement stmt = this
			->db.GetConnection()
			->PrepareStatement("INSERT INTO courses (id, name) VALUES (?, ?);");
		stmt.Bind(1, course.GetId());
		stmt.Bind(2, wxString(course.GetName()));
		return stmt.ExecuteUpdate() > 0;
	} catch(const wxSQLite3::Exception&) {
		return false;
	}
}

bool CourseDbService::UpdateCourse(const Course& course) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("UPDATE courses SET name = ? WHERE id = ?;");

	stmt.Bind(1, wxString(course.GetName()));
	stmt.Bind(2, course.GetId());
	return stmt.ExecuteUpdate() > 0;
}

bool CourseDbService::DeleteCourse(std::string courseID) {
	wxSQLite3::Statement stmt = this
		->db.GetConnection()
		->PrepareStatement("DELETE FROM courses WHERE id = ?;");

	stmt.Bind(1, courseID);
	return stmt.ExecuteUpdate() > 0;
}
