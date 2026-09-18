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

### 2. Build wxWidgets

The app links static MinGW wxWidgets 3.2 libraries (`libwxmsw32u_*.a`).

1. Download wxWidgets 3.2.x from [wxwidgets.org/downloads](https://www.wxwidgets.org/downloads/) and extract it to `C:\wxWidgets`, or set `WXWIN` to your extract folder.
2. From the repo root, run `build_wxWidgets.bat`. Libraries land in `%WXWIN%\lib\gcc_lib` (default `C:\wxWidgets\lib\gcc_lib`).

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

3. `./build.sh` installs `wxwidgets` via Homebrew if `wx-config` is missing. You can also run `brew install wxwidgets` yourself.

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

Then from the repo root:

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
| `wx-config --version` | wxWidgets (macOS / Linux) |

If `g++ --version` fails on Windows after install, the `bin` folder is not on PATH for **this** terminal. Close it and open a new one.
