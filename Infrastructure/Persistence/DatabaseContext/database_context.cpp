#include "database_context.h"
#include <stdexcept>

DatabaseContext::DatabaseContext(const std::string& dbPath) : dbPath(dbPath) {}

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
}

wxSQLite3::Database* DatabaseContext::GetConnection() {
	this->EnsureOpen();
	return this->db.get();
}