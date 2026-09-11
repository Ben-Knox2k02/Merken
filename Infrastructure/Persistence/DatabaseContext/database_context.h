#ifndef DATABASE_CONTEXT_REAL_H
#define DATABASE_CONTEXT_REAL_H

#include <stdexcept>
#include <wx/wxsqlite3.h>
#include <string>
#include <memory>

class DatabaseContext {
	public:
		explicit DatabaseContext(const std::string& dbPath);
		~DatabaseContext();

		DatabaseContext(const DatabaseContext&) = delete;
		DatabaseContext& operator=(const DatabaseContext&) = delete;

		wxSQLite3::Database* GetConnection();

	private:
		void EnsureOpen();

		std::string dbPath;
		std::unique_ptr<wxSQLite3::Database> db;
};

#endif