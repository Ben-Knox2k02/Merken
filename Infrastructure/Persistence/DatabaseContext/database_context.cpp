#include "database_context.h"
#include <wx/wxsqlite3.h>
#include <stdexcept>

DatabaseContext::DatabaseContext(const std::string& dbPath) {
	this->db = std::make_unique<wxSQLite3::Database>();
	try {
		this->db->Open(dbPath);
	} catch (const wxSQLite3::Exception& e) {
		throw std::runtime_error("Failed to open database: " + std::string(e.GetMessage()));
	}
}

DatabaseContext::~DatabaseContext() {
	if (this->db && this->db->IsOpen()) {
        this->db->Close();
    }
}

wxSQLite3::Database* DatabaseContext::GetConnection() const {
    return this->db.get();
}