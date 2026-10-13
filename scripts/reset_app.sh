#!/bin/sh
# Clear decks, cards, daily progress, and the profile.
# schema_migrations stays so shipped migrations are not applied again.
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
db=${1:-"$root/merken.db"}

if ! command -v sqlite3 >/dev/null 2>&1; then
	echo "sqlite3 is required" >&2
	exit 1
fi

if [ ! -f "$db" ]; then
	echo "Database not found: $db" >&2
	exit 1
fi

sqlite3 "$db" <<'SQL'
BEGIN IMMEDIATE;
DELETE FROM cards;
DELETE FROM decks;
DELETE FROM daily_progress;
DELETE FROM user_profile;
COMMIT;
SQL

if [ "$(sqlite3 "$db" "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'sqlite_sequence';")" -eq 1 ]; then
	sqlite3 "$db" "DELETE FROM sqlite_sequence WHERE name IN ('cards', 'decks');"
fi

sqlite3 "$db" <<'SQL'
SELECT 'Reset complete: ' ||
	(SELECT COUNT(*) FROM cards) || ' cards, ' ||
	(SELECT COUNT(*) FROM decks) || ' decks, ' ||
	(SELECT COUNT(*) FROM daily_progress) || ' progress rows, ' ||
	(SELECT COUNT(*) FROM user_profile) || ' profiles.';
SQL
