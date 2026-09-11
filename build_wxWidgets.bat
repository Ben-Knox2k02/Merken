@echo off
setlocal EnableExtensions
cd /d "%~dp0"

rem One-time wxWidgets build of the libraries Merken links.
rem Extra modules stay enabled in setup.h so they can be built later.
rem Example: mingw32-make -f makefile.gcc SHARED=0 BUILD=release wxaui

if defined WXWIN (set "WX=%WXWIN%") else set "WX=C:\wxWidgets"
if "%NUMBER_OF_PROCESSORS%"=="" set NUMBER_OF_PROCESSORS=4

call :find_toolchain
if errorlevel 1 exit /b 1

echo Using compiler:
g++ --version
echo.

if not exist "%WX%\build\msw\makefile.gcc" (
    echo wxWidgets sources not found at %WX%
    echo Extract wxWidgets 3.2.x there, or set WXWIN to your wxWidgets tree.
    exit /b 1
)

set "WX_LIB=%WX%\lib\gcc_lib"
set "WX_READY=1"
if not exist "%WX_LIB%\mswu\wx\setup.h" set "WX_READY=0"
if not exist "%WX_LIB%\libwxmsw32u_core.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxbase32u.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxbase32u_net.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxpng.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxjpeg.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxzlib.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxregexu.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxexpat.a" set "WX_READY=0"

if "%WX_READY%"=="1" (
    echo Used wxWidgets libraries already built, skipping.
    echo Libraries are in: %WX%\lib\gcc_lib
    exit /b 0
)

set "WX_MAKE_FLAGS=SHARED=0 BUILD=release UNICODE=1 MONOLITHIC=0 USE_OPENGL=1 USE_XRC=1 USE_AUI=1 USE_RIBBON=1 USE_STC=1 USE_MEDIA=1 USE_PROPGRID=1 USE_RICHTEXT=1 USE_DEBUGREPORT=1 USE_GLCANVAS=1 USE_LIBPNG=1 USE_LIBJPEG=1 USE_ZLIB=1 USE_EXPAT=1 USE_REGEX=1 USE_TIFF=0 USE_WEBVIEW=0 USE_WEBVIEW_EDGE=0"

pushd "%WX%\build\msw"

rem setup.h must be generated single-threaded before -j (wxWidgets makefile bug).
echo Generating setup.h...
%MAKE_CMD% -f makefile.gcc %WX_MAKE_FLAGS% setup_h
if errorlevel 1 (
    popd
    echo.
    echo *** wxWidgets setup.h generation failed ***
    exit /b 1
)

rem Object dir is not created by setup_h; -j races if it is missing.
if not exist gcc_mswu mkdir gcc_mswu

echo Compiling used libraries with %NUMBER_OF_PROCESSORS% jobs:
echo   wxbase wxcore wxnet png jpeg zlib regex expat
rem Static wxcore does not depend on wxbase or the builtin image libs; name them.
%MAKE_CMD% -j%NUMBER_OF_PROCESSORS% -f makefile.gcc %WX_MAKE_FLAGS% ^
  wxbase wxcore wxnet ^
  ..\..\lib\gcc_lib\libwxpng.a ^
  ..\..\lib\gcc_lib\libwxjpeg.a ^
  ..\..\lib\gcc_lib\libwxzlib.a ^
  ..\..\lib\gcc_lib\libwxregexu.a ^
  ..\..\lib\gcc_lib\libwxexpat.a
if errorlevel 1 (
    popd
    echo.
    echo *** wxWidgets build failed ***
    exit /b 1
)

popd
echo.
echo wxWidgets used libraries are in: %WX%\lib\gcc_lib
echo Next: build.bat
exit /b 0

:find_toolchain
call :toolchain_ok
if not errorlevel 1 exit /b 0

if defined MINGW_HOME if exist "%MINGW_HOME%\bin\g++.exe" (
    set "PATH=%MINGW_HOME%\bin;%PATH%"
    call :toolchain_ok
    if not errorlevel 1 exit /b 0
)

if exist "C:\mingw-w64\bin\g++.exe" set "PATH=C:\mingw-w64\bin;%PATH%"
if exist "C:\mingw-w64\mingw64\bin\g++.exe" set "PATH=C:\mingw-w64\mingw64\bin;%PATH%"
call :toolchain_ok
if not errorlevel 1 exit /b 0

for %%D in (
    "C:\msys64\mingw64\bin"
    "C:\msys64\ucrt64\bin"
    "C:\mingw64\bin"
    "C:\MinGW\bin"
    "C:\TDM-GCC-64\bin"
) do (
    if exist "%%~D\g++.exe" (
        set "PATH=%%~D;%PATH%"
        call :toolchain_ok
        if not errorlevel 1 exit /b 0
    )
)

echo g++ not found on PATH.
echo Put any MinGW-w64 bin folder on PATH, or set MINGW_HOME to the toolchain root.
exit /b 1

:toolchain_ok
g++ --version >nul 2>&1
if errorlevel 1 exit /b 1
set "MAKE_CMD="
mingw32-make --version >nul 2>&1
if not errorlevel 1 set "MAKE_CMD=mingw32-make"
if not defined MAKE_CMD (
    make --version >nul 2>&1
    if not errorlevel 1 set "MAKE_CMD=make"
)
if not defined MAKE_CMD exit /b 1
exit /b 0
