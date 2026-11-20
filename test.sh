#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

OUT=merken_tests
echo "Cleaning previous test artifacts..."
rm -f "$OUT" "${OUT}.exe"

echo "Building $OUT"

mkdir -p obj
INC="-IThirdParty/doctest"

compile_cpp() {
	local src=$1
	local obj="obj/$(basename "${src%.cpp}").o"
	echo "$src"
	g++ -std=c++17 -c "$src" $INC -o "$obj"
}

compile_cpp Tests/main.cpp
while IFS= read -r file; do
	compile_cpp "$file"
done < <(find Tests Application -name '*.cpp' ! -name 'main.cpp')
compile_cpp Infrastructure/GoogleCalendar/google_calendar_service.cpp
compile_cpp Infrastructure/GoogleAuth/google_pkce.cpp
compile_cpp Infrastructure/GoogleAuth/google_client_config.cpp
compile_cpp Infrastructure/GoogleAuth/google_auth_service.cpp

echo "Linking..."
g++ obj/main.o obj/google_calendar_service.o obj/google_pkce.o obj/google_client_config.o obj/google_auth_service.o $(find obj \( -name '*_tests.o' -o -name '*_usecase.o' \) | sort) -o "$OUT"

echo "Running $OUT"
"./$OUT"
