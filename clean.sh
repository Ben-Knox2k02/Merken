#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

echo "Cleaning build artifacts..."

rm_if_exists() {
	local path=$1
	if [[ -e $path || -L $path ]]; then
		echo "  $path"
		rm -rf "$path"
	fi
}

rm_if_exists obj
rm_if_exists Merken
rm_if_exists Merken.exe
rm_if_exists Merken.app
rm_if_exists merken_tests
rm_if_exists merken_tests.exe
rm_if_exists UI/src/manifest.rc
rm_if_exists src/manifest.rc

shopt -s nullglob
for f in *.ilk *.pdb *.o; do
	echo "  $f"
	rm -f "$f"
done

echo "Done."
