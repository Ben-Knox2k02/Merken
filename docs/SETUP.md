# C++ environment setup

Merken is a C++17 wxWidgets app. Builds call `g++` (and on Windows, `gcc`). Visual Studio `cl.exe` is not used.

If Windows prints `g++ not found on PATH` or `'g++' is not recognized`, the compiler is missing from PATH.

---

## Windows

You need MinGW-w64 (`g++`, `gcc`, `windres`, `mingw32-make`) and wxWidgets 3.2.x built with that same compiler.

### 1. Install g++

MSYS2 is the same toolchain family CI uses.

1. Install MSYS2 from [msys2.org](https://www.msys2.org/) or run:

   ```
   winget install -e --id MSYS2.MSYS2
   ```

2. Open **MSYS2 UCRT64** from the Start menu and install the compiler:

   ```
   pacman -S --needed mingw-w64-ucrt-x86_64-toolchain
   ```

   CI uses the MINGW64 environment instead:

   ```
   pacman -S --needed mingw-w64-x86_64-toolchain
   ```

   Either works. Use the **same** `g++` for `build_wxWidgets.bat` and `build.bat`.

3. Add the matching `bin` folder to your **user** PATH:

   | Environment | Folder to add |
   |---|---|
   | UCRT64 | `C:\msys64\ucrt64\bin` |
   | MINGW64 | `C:\msys64\mingw64\bin` |

   Search **Edit environment variables for your account**, edit `Path`, and add that folder.

4. Close every terminal. Open a new Command Prompt or PowerShell and check:

   ```
   g++ --version
   gcc --version
   ```

Standalone MinGW-w64 (for example [WinLibs](https://winlibs.com/)) also works: put its `mingw64\bin` on PATH, or set `MINGW_HOME` to the toolchain root (`bin\g++.exe` must exist under that root).

`build.bat` and `build_wxWidgets.bat` also look in common install folders. `test.bat` needs `g++` on PATH.

### 2. Install and build wxWidgets

`build.bat` links **static** MinGW wxWidgets **3.2** (`libwxmsw32u_*.a`). Pre-built Visual Studio binaries and wxWidgets 3.3 will not work. Do this after `g++` is on PATH.

1. Download the **Windows source** archive `wxWidgets-3.2.x.zip` (or `.7z`) from [wxwidgets.org/downloads](https://www.wxwidgets.org/downloads/) or the [3.2 GitHub releases](https://github.com/wxWidgets/wxWidgets/releases).

   Do **not** use the GitHub “Source code” zip (it is missing third-party sources). Do **not** use `wxMSW-*-Setup.exe` as an installed binary — it is sources only, and this project expects the unzipped tree.

2. Extract so this file exists:

   ```
   C:\wxWidgets\build\msw\makefile.gcc
   ```

   If the zip created `C:\wxWidgets\wxWidgets-3.2.x\`, either move the inner folder’s contents up to `C:\wxWidgets` or set `WXWIN` to that inner folder. Avoid spaces in the path.

   To use a different location (new terminal after this):

   ```
   setx WXWIN C:\path\to\wxWidgets
   ```

3. From the **Merken repo root** in Command Prompt or PowerShell (not an MSYS2 bash shell):

   ```
   build_wxWidgets.bat
   ```

   The first run takes several minutes. It uses `mingw32-make` from the MinGW-w64 toolchain. Success prints libraries in `C:\wxWidgets\lib\gcc_lib` (or `%WXWIN%\lib\gcc_lib`). Later runs skip if those libs are already there.

4. Confirm these files exist:

   ```
   C:\wxWidgets\lib\gcc_lib\libwxmsw32u_core.a
   C:\wxWidgets\lib\gcc_lib\libwxbase32u.a
   C:\wxWidgets\lib\gcc_lib\libwxbase32u_net.a
   C:\wxWidgets\lib\gcc_lib\mswu\wx\setup.h
   ```

If you change compiler (UCRT64 vs MINGW64, or a new MinGW), delete `%WXWIN%\lib\gcc_lib` and `%WXWIN%\build\msw\gcc_mswu` and run `build_wxWidgets.bat` again with the same `g++` you will use for `build.bat`.

### 3. Build Merken

From the repo root:

```
build.bat
test.bat
```

`build.bat` produces `Merken.exe`. `test.bat` produces and runs `merken_tests.exe`.

---

## macOS

1. Install [Homebrew](https://brew.sh) if you do not have it.
2. Install Xcode command-line tools so `g++` / `clang++` and `cc` exist:

   ```
   xcode-select --install
   ```

3. Install wxWidgets 3.2 (provides `wx-config`):

   ```
   brew install wxwidgets
   ```

   `./build.sh` runs that Homebrew install for you if `wx-config` is missing.

```
./build.sh
./test.sh
```

Do not use `sudo`. On success you get `Merken.app`.

---

## Linux

Install a C++17 compiler and wxWidgets 3.2 (Debian/Ubuntu):

```
sudo apt-get update
sudo apt-get install -y build-essential pkg-config libwxgtk3.2-dev
```

`libwxgtk3.2-dev` provides `wx-config`. Then from the repo root:

```
./build.sh
./test.sh
```

`wx-config` must be on PATH. The binary is `./Merken`.

---

## Check the toolchain

| Command | What it proves |
|---|---|
| `g++ --version` | C++ compiler is on PATH |
| `gcc --version` | C compiler (Windows app build compiles a `.c` file with `gcc`) |
| `mingw32-make --version` | Make used by `build_wxWidgets.bat` (Windows) |
| `wx-config --version` | wxWidgets (macOS / Linux) |
| `dir C:\wxWidgets\lib\gcc_lib\libwxmsw32u_core.a` | wxWidgets static libs (Windows; use `%WXWIN%` if you set it) |

If `g++ --version` fails on Windows after install, the `bin` folder is not on PATH for **this** terminal. Close it and open a new one.

`test.bat` / `./test.sh` need `g++` only. The app build also needs wxWidgets (`build_wxWidgets.bat` on Windows, `wx-config` on macOS / Linux).
