@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo Cleaning build artifacts...

if exist obj (
    echo   obj\
    rmdir /s /q obj
)

for %%F in (
    Merken.exe
    merken_tests.exe
    Merken
    merken_tests
    UI\src\manifest.rc
    src\manifest.rc
) do (
    if exist "%%F" (
        echo   %%F
        del /q "%%F"
    )
)

if exist Merken.app (
    echo   Merken.app\
    rmdir /s /q Merken.app
)

del /q *.ilk 2>nul
del /q *.pdb 2>nul

echo Done.
exit /b 0
