#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

OUT=Merken
echo "Building $OUT"

if [[ "$(uname -s)" == "Darwin" ]]; then
	if ! command -v brew >/dev/null 2>&1; then
		echo "Homebrew not found. Install it from https://brew.sh then run this again." >&2
		exit 1
	fi
	echo "Installing wxwidgets (no-op if already installed)..."
	brew install wxwidgets
fi

if ! command -v wx-config >/dev/null 2>&1; then
	echo "wx-config not found." >&2
	exit 1
fi

mkdir -p obj

INC="-IUI/include -IThirdParty/boost-di -IThirdParty/wxSQLite3/include -IThirdParty/wxSQLite3/src $(wx-config --cxxflags)"

compile_cpp() {
	local src=$1
	local obj="obj/$(basename "${src%.cpp}").o"
	echo "$src"
	g++ -std=c++17 -c "$src" $INC -o "$obj"
}

while IFS= read -r file; do
	compile_cpp "$file"
done < <(find Infrastructure Application -name '*.cpp')

for file in UI/src/*.cpp; do
	compile_cpp "$file"
done

compile_cpp ThirdParty/wxSQLite3/src/wxsqlite3.cpp

echo "ThirdParty/wxSQLite3/src/sqlite3mc_amalgamation.c"
cc -c ThirdParty/wxSQLite3/src/sqlite3mc_amalgamation.c -IThirdParty/wxSQLite3/src -o obj/sqlite3mc_amalgamation.o

EXTRALIBS="-lpthread"
if [[ "$(uname -s)" == "Linux" ]]; then
	EXTRALIBS="$EXTRALIBS -ldl"
fi

echo "Linking..."
g++ obj/*.o $(wx-config --libs) $EXTRALIBS -o "$OUT"

if [[ "$(uname -s)" == "Darwin" ]]; then
	APP="$OUT.app"
	MACOS_DIR="$APP/Contents/MacOS"
	mkdir -p "$MACOS_DIR"

	mv "$OUT" "$MACOS_DIR/merken-bin"

	cat > "$MACOS_DIR/$OUT" << 'EOF'
#!/bin/bash
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT"
exec "$(dirname "$0")/merken-bin" "$@"
EOF
	chmod +x "$MACOS_DIR/$OUT" "$MACOS_DIR/merken-bin"

	cat > "$APP/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>$OUT</string>
	<key>CFBundleIdentifier</key>
	<string>com.merken.app</string>
	<key>CFBundleName</key>
	<string>$OUT</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleVersion</key>
	<string>1.0</string>
	<key>NSHighResolutionCapable</key>
	<true/>
</dict>
</plist>
EOF

	echo "Built ./$APP"
	echo "Double-click $APP to run."
else
	echo "Built ./$OUT"
	echo "Run from this directory: ./$OUT"
fi
