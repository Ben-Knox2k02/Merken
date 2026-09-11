#include <wx/wxsqlite3.h>
#include <string>
#include <memory>
#include <stdexcept>

#ifndef DATABASE_CONTEXT_REAL_H
#define DATABASE_CONTEXT_REAL_H

class DatabaseContext {
	public:
		DatabaseContext(const std::string& dbPath);
		~DatabaseContext();

        // Prevents copying and assignment of the DatabaseContext instance.
        DatabaseContext(const DatabaseContext&) = delete;
        DatabaseContext& operator=(const DatabaseContext&) = delete;

        // Returns a pointer to the underlying wxSQLite3::Database instance.
		wxSQLite3::Database* GetConnection() const;

	private:
		std::unique_ptr<wxSQLite3::Database> db;
};

#endif