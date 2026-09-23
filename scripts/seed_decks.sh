#!/bin/sh
# Insert 12 sample decks with 40 cards each. Existing decks are left in place.
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
db=${1:-"$root/merken.db"}

if ! command -v python3 >/dev/null 2>&1; then
	echo "python3 is required" >&2
	exit 1
fi

if [ ! -f "$db" ]; then
	echo "Database not found: $db" >&2
	echo "Open Merken once so it can create the database, then run this again." >&2
	exit 1
fi

python3 "$root/scripts/seed_decks.py" "$db"
