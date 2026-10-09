@echo off
setlocal enabledelayedexpansion

echo Custom Archive File libcomctl32_subclass.a should be installed in C:\path\to\your\MinGW\lib

rem === OUTPUT ==========================================================
set OUT=Merken.exe
echo Building %OUT%

rem === PATHS ===========================================================
if defined MINGW (
    set "MINGW=%MINGW%"
) else if exist "C:\msys64\mingw64\bin\g++.exe" (
    set "MINGW=C:\msys64\mingw64"
) else if exist "C:\mingw64\bin\g++.exe" (
    set "MINGW=C:\mingw64"
) else if exist "C:\mingw-w64\bin\g++.exe" (
    set "MINGW=C:\mingw-w64"
) else (
    set "MINGW=C:\mingw64"
)

if defined WXWIN (
    set "WX=%WXWIN%"
) else if exist "C:\wxWidgets" (
    set "WX=C:\wxWidgets"
) else (
    set "WX=C:\wxWidgets"
)

set PATH=%MINGW%\bin;%PATH%

rem === CONFIRM ENVIRONMENT =============================================
echo Using MinGW path: %MINGW%
echo Using wxWidgets path: %WX%

rem === INCLUDE / LIB PATHS =============================================
set INC=-I UI\include -I ThirdParty\boost-di -I ThirdParty\wxSQLite3\include -I ThirdParty\wxSQLite3\src -I %WX%\lib\gcc_lib\mswu -I %WX%\include
set LIB=-L %WX%\lib\gcc_lib -L %MINGW%\lib

rem === WXWIDGETS LIBS (wxWidgets 3.3.x) =================================
set WXLIBS= ^
    -lwxmsw33u_core ^
    -lwxbase33u_net ^
    -lwxbase33u ^
    -lwxpng ^
    -lwxjpeg ^
    -lwxwebp ^
    -lwxtiff ^
    -lwxzlib ^
    -lwxregexu ^
    -lwxexpat

rem === WINDOWS SYSTEM LIBS =============================================
set WINLIBS= ^
    -lshlwapi ^
    -lversion ^
    -lole32 ^
    -lshell32 ^
    -luuid ^
    -lrpcrt4 ^
    -luxtheme ^
    -lgdi32 ^
    -loleaut32 ^
    -lcomdlg32 ^
    -lcomctl32 ^
	-lcomctl32_subclass ^
    -loleacc ^
    -lwinspool ^
    -lwininet ^
    -lws2_32 ^
    -lcrypt32 ^
    -lgdiplus ^
    -lmsimg32

rem === ENSURE OBJ FOLDER ===============================================
if exist obj rmdir /s /q obj
mkdir obj

rem ==== ICON ============================================================
echo Compiling icon resource...
if exist UI\resources\app.rc (
     %MINGW%\bin\windres.exe UI\resources\app.rc -O coff -o obj\app_icon.o
) else (
     echo Using Default Icon
)

rem ==== MANIFEST ===========================================
echo Compiling manifest...
if exist UI\resources\manifest.rc (
    %MINGW%\bin\windres.exe UI\resources\manifest.rc -O coff -o obj\manifest.o
) else (
    echo Using Default Manifest
)

rem === PARALLEL COMPILE (UI + Application + ThirdParty) ==================
echo Compiling sources...

for /r %%f in (*.cpp) do (
    set "filepath=%%f"
    echo !filepath! | findstr /i "ThirdParty Tests" >nul
    if errorlevel 1 (
        start /B cmd /c %MINGW%\bin\g++ -std=c++17 -pipe -O2 -c "%%f" %INC% -o obj\%%~nf.o
    )
)

rem === Compile wxSQLite3 wrapper files =================================
if exist ThirdParty\wxSQLite3\src\wxsqlite3.cpp (
    start /B cmd /c %MINGW%\bin\g++ -std=c++17 -pipe -O2 -c ThirdParty\wxSQLite3\src\wxsqlite3.cpp %INC% -o obj\wxsqlite3.o
)
if exist ThirdParty\wxSQLite3\src\sqlite3mc_amalgamation.c (
    start /B cmd /c %MINGW%\bin\gcc -pipe -O2 -w -c ThirdParty\wxSQLite3\src\sqlite3mc_amalgamation.c -I ThirdParty\wxSQLite3\src -o obj\sqlite3mc_amalgamation.o
)

rem === WAIT FOR PARALLEL JOBS ==========================================
echo Waiting for compilation jobs...
:waitloop
tasklist /FI "IMAGENAME eq g++.exe" 2>nul | find /I "g++.exe" >nul
if not errorlevel 1 (
    timeout /t 1 >nul
    goto waitloop
)
tasklist /FI "IMAGENAME eq gcc.exe" 2>nul | find /I "gcc.exe" >nul
if not errorlevel 1 (
    timeout /t 1 >nul
    goto waitloop
)

rem === LINK =============================================================
echo Linking...
%MINGW%\bin\g++ obj\*.o -o %OUT% %LIB% %WXLIBS% %WINLIBS%
if errorlevel 1 (
    echo.
    echo Link failed.
    exit /b 1
)

echo Build complete: %OUT%
endlocal
