#include "db_scripts.h"

// Append a new SQL string. Do not edit or reorder scripts that already shipped.
std::vector<std::string> DbScripts::GetScripts() const {
	return {
		R"(
			CREATE TABLE IF NOT EXISTS decks (
				deck_id INTEGER PRIMARY KEY AUTOINCREMENT,
				name TEXT NOT NULL,
				description TEXT NOT NULL DEFAULT '',
				created_at TEXT NOT NULL DEFAULT ''
			);
		)",
	};
}
