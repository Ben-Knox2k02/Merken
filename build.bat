@echo off
setlocal enabledelayedexpansion

rem === OUTPUT ==========================================================
set OUT=Merken.exe
echo Building %OUT%

rem === PATHS ===========================================================
rem MINGW and MINGW_HOME are the toolchain root: the folder that contains bin\g++.exe.
if defined MINGW if exist "%MINGW%\bin\g++.exe" goto :mingw_ready
if defined MINGW_HOME if exist "%MINGW_HOME%\bin\g++.exe" (
    set "MINGW=%MINGW_HOME%"
    goto :mingw_ready
)

set "MINGW="
for %%D in (
    "C:\msys64\ucrt64"
    "C:\msys64\mingw64"
    "C:\msys64\mingw-w64"
    "C:\mingw64"
    "C:\mingw-w64\mingw64"
    "C:\mingw-w64"
    "C:\MinGW"
    "C:\TDM-GCC-64"
) do (
    if not defined MINGW if exist "%%~D\bin\g++.exe" set "MINGW=%%~D"
)

if not defined MINGW (
    for /f "delims=" %%G in ('where g++ 2^>nul') do (
        if not defined MINGW for %%I in ("%%~dpG..") do set "MINGW=%%~fI"
    )
)

:mingw_ready
if not exist "%MINGW%\bin\g++.exe" (
    echo MinGW g++ was not found.
    echo Set MINGW to the toolchain root, the folder that contains bin\g++.exe.
    exit /b 1
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
rem wx include directories come before wxSQLite3. That tree has its own wx\
rem folder, and g++ treats a failed open there as a hard error instead of
rem continuing on to the real wx headers.
set INC=-I %WX%/lib/gcc_lib/mswu -I %WX%/include -I UI/include -I ThirdParty/boost-di -I ThirdParty/wxSQLite3/include -I ThirdParty/wxSQLite3/src
rem Repo root is searched first so the committed libcomctl32_subclass.a wins.
set LIB=-L "%~dp0." -L %WX%\lib\gcc_lib -L %MINGW%\lib

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
if not exist obj mkdir obj
if not exist obj\manifest mkdir obj\manifest

rem ==== ICON ============================================================
echo Compiling icon resource...
if exist UI\resources\app.rc (
     %MINGW%\bin\windres.exe UI\resources\app.rc -O coff -o obj\app_icon.o
)

rem ==== MANIFEST ========================================================
echo Compiling manifest...
if exist UI\resources\manifest.rc (
    %MINGW%\bin\windres.exe UI\resources\manifest.rc -O coff -o obj\manifest\default-manifest.o
)

rem === INCREMENTAL PARALLEL COMPILE ======================================
echo Compiling sources...

for /r %%f in (*.cpp) do (
    set "filepath=%%f"
    echo !filepath! | findstr /i "ThirdParty Tests" >nul
    if errorlevel 1 (
        set "src=%%f"
        set "obj=obj\%%~nf.o"
        
        rem Check if object file doesn't exist or is older than source file
        if not exist "!obj!" (
            start /B cmd /c %MINGW%\bin\g++ -std=c++17 -pipe -O2 -c "!src!" %INC% -o "!obj!"
        ) else (
            for %%i in ("!src!") do set "srctime=%%~ti"
            for %%i in ("!obj!") do set "objtime=%%~ti"
            if "!srctime!" gtr "!objtime!" (
                start /B cmd /c %MINGW%\bin\g++ -std=c++17 -pipe -O2 -c "!src!" %INC% -o "!obj!"
            )
        )
    )
)

rem === Compile wxSQLite3 wrapper files incrementally =====================
if exist ThirdParty\wxSQLite3\src\wxsqlite3.cpp (
    if not exist obj\wxsqlite3.o (
        start /B cmd /c %MINGW%\bin\g++ -std=c++17 -pipe -O2 -c ThirdParty\wxSQLite3\src\wxsqlite3.cpp %INC% -o obj\wxsqlite3.o
    )
)
if exist ThirdParty\wxSQLite3\src\sqlite3mc_amalgamation.c (
    if not exist obj\sqlite3mc_amalgamation.o (
        start /B cmd /c %MINGW%\bin\gcc -pipe -O2 -w -c ThirdParty\wxSQLite3\src\sqlite3mc_amalgamation.c -I ThirdParty\wxSQLite3\src -o obj\sqlite3mc_amalgamation.o
    )
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
set "MANIFEST_B="
if exist obj\manifest\default-manifest.o set "MANIFEST_B=%CD%\obj\manifest"
if defined MANIFEST_B set "MANIFEST_B=%MANIFEST_B:\=/%/"
if defined MANIFEST_B (
    %MINGW%\bin\g++ obj\*.o -o %OUT% -B "%MANIFEST_B%" %LIB% %WXLIBS% %WINLIBS%
) else (
    %MINGW%\bin\g++ obj\*.o -o %OUT% %LIB% %WXLIBS% %WINLIBS%
)
if errorlevel 1 (
    echo.
    echo Link failed.
    exit /b 1
)

echo Build complete: %OUT%
endlocal
