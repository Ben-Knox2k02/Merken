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
		R"(
CREATE TABLE IF NOT EXISTS daily_progress (
    date TEXT PRIMARY KEY,
    cards_reviewed INTEGER NOT NULL DEFAULT 0,
    cards_correct INTEGER NOT NULL DEFAULT 0
);
)",
		R"(
			CREATE TABLE IF NOT EXISTS user_profile (
				profile_id INTEGER PRIMARY KEY CHECK (profile_id = 1),
				display_name TEXT NOT NULL DEFAULT '',
				note TEXT NOT NULL DEFAULT '',
				image_path TEXT NOT NULL DEFAULT '',
				start_date TEXT NOT NULL,
				daily_goal INTEGER,
				uses_ai_study INTEGER NOT NULL DEFAULT 0,
				guide_finished INTEGER NOT NULL DEFAULT 0
			);
		)",
		R"(
			ALTER TABLE user_profile DROP COLUMN image_path;
		)",
		R"(
			ALTER TABLE user_profile ADD COLUMN onboarding_finished INTEGER NOT NULL DEFAULT 0;
		)",
		R"(
CREATE TABLE IF NOT EXISTS review_events (
    review_id INTEGER PRIMARY KEY AUTOINCREMENT,
    deck_id INTEGER NOT NULL,
    card_id INTEGER NOT NULL,
    reviewed_on TEXT NOT NULL,
    remembered INTEGER NOT NULL,
    interval_days INTEGER NOT NULL,
    ease_factor REAL NOT NULL,
    repetition_count INTEGER NOT NULL,
    next_review_date TEXT NOT NULL DEFAULT '',
    FOREIGN KEY (deck_id) REFERENCES decks(deck_id),
    FOREIGN KEY (card_id) REFERENCES cards(card_id)
);
CREATE INDEX IF NOT EXISTS idx_cards_deck_next_review_date
    ON cards(deck_id, next_review_date, card_id);
CREATE INDEX IF NOT EXISTS idx_review_events_card_date
    ON review_events(card_id, reviewed_on, review_id);
CREATE INDEX IF NOT EXISTS idx_review_events_deck_date
    ON review_events(deck_id, reviewed_on, review_id);
CREATE INDEX IF NOT EXISTS idx_review_events_date
    ON review_events(reviewed_on, review_id);
)",
	};
}
