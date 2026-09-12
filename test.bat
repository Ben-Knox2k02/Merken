@echo off
setlocal EnableExtensions
cd /d "%~dp0"

set OUT=merken_tests.exe
if not exist obj mkdir obj

echo Building %OUT%

g++ -std=c++17 -c Tests\main.cpp -I ThirdParty\doctest -o obj\main.o
if errorlevel 1 exit /b 1

for /R Tests %%F in (*_tests.cpp) do (
    echo %%F
    g++ -std=c++17 -c "%%F" -I ThirdParty\doctest -o "obj\%%~nF.o"
    if errorlevel 1 exit /b 1
)

for /R Application %%F in (*.cpp) do (
    echo %%F
    g++ -std=c++17 -c "%%F" -I ThirdParty\doctest -o "obj\%%~nF.o"
    if errorlevel 1 exit /b 1
)

echo Linking...
g++ obj\main.o obj\*_tests.o obj\*_usecase.o -o %OUT%
if errorlevel 1 exit /b 1

echo Running %OUT%
%OUT%
exit /b %ERRORLEVEL%
