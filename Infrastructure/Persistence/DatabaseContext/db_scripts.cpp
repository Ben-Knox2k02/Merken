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
		R"(
			CREATE TABLE IF NOT EXISTS cards (
				card_id INTEGER PRIMARY KEY AUTOINCREMENT,
				deck_id INTEGER NOT NULL,
				front TEXT NOT NULL,
				back TEXT NOT NULL,
				tags TEXT NOT NULL DEFAULT '',
				card_type INTEGER NOT NULL DEFAULT 0,
				choices TEXT NOT NULL DEFAULT '',
				interval_days INTEGER NOT NULL DEFAULT 0,
				ease_factor REAL NOT NULL DEFAULT 2.5,
				repetition_count INTEGER NOT NULL DEFAULT 0,
				next_review_date TEXT NOT NULL DEFAULT '',
				last_reviewed_date TEXT NOT NULL DEFAULT '',
				FOREIGN KEY (deck_id) REFERENCES decks(deck_id)
			);
		)",
	};
}
