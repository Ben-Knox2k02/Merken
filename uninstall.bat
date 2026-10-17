@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo Uninstalling Merken generated data and artifacts...
call "%~dp0clean.bat"

echo Removing app data...

for %%F in (
    merken.db
    merken.db-journal
    merken.db-wal
    merken.db-shm
    app_settings.json
    settings.bin
    settings.json
) do (
    if exist "%%F" (
        echo   %%F
        del /q "%%F"
    )
)

if defined APPDATA if exist "%APPDATA%\Merken" (
    echo   %APPDATA%\Merken\
    rmdir /s /q "%APPDATA%\Merken"
)

echo Done.
exit /b 0
