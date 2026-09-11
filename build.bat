@echo off
setlocal EnableExtensions
cd /d "%~dp0"

rem Incremental Merken build. Recompiles only sources newer than their .o.
rem sqlite amalgamation is compiled once. Run build_wxWidgets.bat first.

set OUT=Merken.exe
if defined WXWIN (set "WX=%WXWIN%") else set "WX=C:\wxWidgets"
if "%NUMBER_OF_PROCESSORS%"=="" set NUMBER_OF_PROCESSORS=4

call :find_toolchain
if errorlevel 1 exit /b 1

echo Using compiler:
g++ --version
echo.

set "WX_LIB=%WX%\lib\gcc_lib"
set "WX_READY=1"
if not exist "%WX_LIB%\libwxmsw32u_core.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxbase32u.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxbase32u_net.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxpng.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxjpeg.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxzlib.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxregexu.a" set "WX_READY=0"
if not exist "%WX_LIB%\libwxexpat.a" set "WX_READY=0"
if "%WX_READY%"=="0" (
    echo wxWidgets libraries incomplete at %WX_LIB%
    echo Run build_wxWidgets.bat first, or set WXWIN to your wxWidgets tree.
    exit /b 1
)

set INC=-I UI\include -I ThirdParty\boost-di -I ThirdParty\wxSQLite3\include -I ThirdParty\wxSQLite3\src -I %WX%\lib\gcc_lib\mswu -I %WX%\include
set LIB=-L %WX%\lib\gcc_lib
set WXLIBS=-lwxmsw32u_core -lwxbase32u_net -lwxbase32u -lwxpng -lwxjpeg -lwxzlib -lwxregexu -lwxexpat
set WINLIBS=-lshlwapi -lversion -lole32 -lshell32 -luuid -lrpcrt4 -luxtheme -lgdi32 -loleaut32 -lcomdlg32 -lcomctl32 -loleacc -lwinspool -lwininet -lws2_32

if not exist obj mkdir obj

echo Building %OUT%
echo Compiling stale sources...

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$headerMax = $null;" ^
  "$hdrs = Get-ChildItem -Recurse UI,Application,DomainModels,Infrastructure,DiComposition -Include *.h,*.hpp -ErrorAction SilentlyContinue;" ^
  "if ($hdrs) { $headerMax = ($hdrs | Measure-Object LastWriteTime -Maximum).Maximum };" ^
  "function OutOfDate([string]$s,[string]$o,[bool]$checkHdr){ if (-not (Test-Path $o)) { return $true }; $ot = (Get-Item -LiteralPath $o).LastWriteTime; if ((Get-Item -LiteralPath $s).LastWriteTime -gt $ot) { return $true }; if ($checkHdr -and $headerMax -and $headerMax -gt $ot) { return $true }; return $false };" ^
  "function Compile([string]$src,[string]$obj,[string]$cc,[string[]]$extra,[bool]$checkHdr){" ^
  "  if (-not (OutOfDate $src $obj $checkHdr)) { Write-Host ('skip ' + $src); return 0 };" ^
  "  Write-Host $src;" ^
  "  if ($cc -eq 'gcc') { & gcc @extra -c $src -o $obj } else { & g++ -std=c++17 @extra -c $src -o $obj };" ^
  "  return $LASTEXITCODE" ^
  "};" ^
  "$inc = @($env:INC -split ' +' | Where-Object { $_ });" ^
  "Get-ChildItem -Recurse Infrastructure,Application,'UI\src' -Filter *.cpp | ForEach-Object {" ^
  "  $code = Compile $_.FullName ('obj\' + $_.BaseName + '.o') 'g++' $inc $true;" ^
  "  if ($code -ne 0) { exit $code }" ^
  "};" ^
  "$code = Compile 'ThirdParty\wxSQLite3\src\wxsqlite3.cpp' 'obj\wxsqlite3.o' 'g++' $inc $false; if ($code -ne 0) { exit $code };" ^
  "$code = Compile 'ThirdParty\wxSQLite3\src\sqlite3mc_amalgamation.c' 'obj\sqlite3mc_amalgamation.o' 'gcc' @('-I','ThirdParty\wxSQLite3\src','-w') $false; if ($code -ne 0) { exit $code }"
if errorlevel 1 (
    echo.
    echo Compile failed.
    exit /b 1
)

rem GCC 16 already embeds a default manifest; a second one from windres fails the link.
if exist obj\manifest.o del obj\manifest.o
if exist UI\src\manifest.rc del UI\src\manifest.rc

echo Linking...
g++ obj\*.o -o %OUT% %LIB% %WXLIBS% %WINLIBS%
if errorlevel 1 (
    echo.
    echo Link failed.
    exit /b 1
)

echo Build complete: %OUT%
echo Run it from this folder: %OUT%
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
gcc --version >nul 2>&1
if errorlevel 1 exit /b 1
windres --version >nul 2>&1
if errorlevel 1 exit /b 1
set "MAKE_CMD="
mingw32-make --version >nul 2>&1
if not errorlevel 1 set "MAKE_CMD=mingw32-make"
if not defined MAKE_CMD (
    make --version >nul 2>&1
    if not errorlevel 1 set "MAKE_CMD=make"
)
exit /b 0
