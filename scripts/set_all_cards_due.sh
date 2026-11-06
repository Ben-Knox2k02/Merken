#!/bin/sh
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
UPDATE cards
SET next_review_date = '';

SELECT COUNT(*) || ' cards are due across ' ||
	(SELECT COUNT(DISTINCT deck_id) FROM cards) || ' decks.'
FROM cards
WHERE next_review_date = '';
SQL
