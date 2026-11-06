#!/bin/bash
cd "$(dirname "$0")"

./build.sh
status=$?

echo
if [[ $status -eq 0 && -d Merken.app ]]; then
	echo "Merken.app is ready. Opening it..."
	open Merken.app
else
	echo "Build failed."
fi

echo
read -n 1 -s -p "Press any key to close..."
exit $status
