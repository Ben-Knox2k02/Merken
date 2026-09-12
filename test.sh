#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

OUT=merken_tests
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

echo "Linking..."
g++ obj/main.o $(find obj \( -name '*_tests.o' -o -name '*_usecase.o' \) | sort) -o "$OUT"

echo "Running $OUT"
"./$OUT"
