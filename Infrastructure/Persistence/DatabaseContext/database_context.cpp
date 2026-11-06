#include "database_context.h"
#include "db_scripts.h"
#include <stdexcept>

DatabaseContext::DatabaseContext(const std::string& dbPath) : dbPath(dbPath) {
	this->EnsureOpen();
}

DatabaseContext::~DatabaseContext() {
	if (this->db && this->db->IsOpen()) {
		this->db->Close();
	}
}

void DatabaseContext::EnsureOpen() {
	if (this->db && this->db->IsOpen()) {
		return;
	}

	this->db = std::make_unique<wxSQLite3::Database>();
	try {
		this->db->Open(
			this->dbPath,
			wxEmptyString,
			wxSQLite3::OPEN_READWRITE | wxSQLite3::OPEN_CREATE
		);
	} catch (const wxSQLite3::Exception& e) {
		this->db.reset();
		throw std::runtime_error("Failed to open database: " + std::string(e.GetMessage()));
	}

	try {
		this->ApplyMigrations();
	} catch (const wxSQLite3::Exception& e) {
		this->db.reset();
		throw std::runtime_error("Failed to apply database migrations: " + std::string(e.GetMessage()));
	}
}

void DatabaseContext::ApplyMigrations() {
	this->db->ExecuteUpdate(
		"CREATE TABLE IF NOT EXISTS schema_migrations ("
		"id INTEGER PRIMARY KEY NOT NULL"
		");"
	);

	DbScripts dbScripts;
	const std::vector<std::string> scripts = dbScripts.GetScripts();
	for (size_t i = 0; i < scripts.size(); ++i) {
		this->RunMigration(static_cast<int>(i + 1), scripts[i]);
	}
}

void DatabaseContext::RunMigration(int version, const std::string& sql) {
	{
		wxSQLite3::Statement stmt = this->db->PrepareStatement(
			"SELECT 1 FROM schema_migrations WHERE id = ?;"
		);
		stmt.Bind(1, version);
		wxSQLite3::ResultSet result = stmt.ExecuteQuery();
		if (result.NextRow()) {
			return;
		}
	}

	wxSQLite3::Transaction transaction(this->db.get());
	this->db->ExecuteUpdate(sql.c_str());
	wxSQLite3::Statement insert = this->db->PrepareStatement(
		"INSERT INTO schema_migrations (id) VALUES (?);"
	);
	insert.Bind(1, version);
	insert.ExecuteUpdate();
	transaction.Commit();
}

wxSQLite3::Database* DatabaseContext::GetConnection() {
	this->EnsureOpen();
	return this->db.get();
}