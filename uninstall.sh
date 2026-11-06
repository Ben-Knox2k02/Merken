#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

# App data lives under the real user's HOME. If this was started with sudo,
# drop back so we do not look in /var/root.
if [[ ${EUID:-$(id -u)} -eq 0 ]]; then
	target="${SUDO_USER:-}"
	if [[ -z "$target" || "$target" == "root" ]]; then
		if [[ "$(uname -s)" == "Darwin" ]]; then
			target="$(stat -f '%Su' .)"
		else
			target="$(stat -c '%U' .)"
		fi
	fi
	if [[ -n "$target" && "$target" != "root" ]]; then
		echo "Re-running as $target (do not use sudo with this script)."
		exec sudo -u "$target" -H -- "$0" "$@"
	fi
	echo "Do not run this script as root. Run ./uninstall.sh without sudo." >&2
	exit 1
fi

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
