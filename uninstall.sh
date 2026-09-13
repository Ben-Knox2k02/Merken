#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

echo "Uninstalling Merken generated data and artifacts..."
bash ./clean.sh

echo "Removing app data..."

rm_if_exists() {
	local path=$1
	if [[ -e $path || -L $path ]]; then
		echo "  $path"
		rm -rf "$path"
	fi
}

rm_if_exists merken.db
rm_if_exists merken.db-journal
rm_if_exists merken.db-wal
rm_if_exists merken.db-shm
rm_if_exists app_settings.json
rm_if_exists settings.bin
rm_if_exists settings.json

rm_if_exists "$HOME/.Merken"
rm_if_exists "${XDG_CONFIG_HOME:-$HOME/.config}/Merken"
rm_if_exists "$HOME/Library/Application Support/Merken"

echo "Done."
