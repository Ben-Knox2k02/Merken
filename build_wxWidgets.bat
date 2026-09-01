@echo off
setlocal

rem --- TOOLCHAIN ------------------------------------------------------

set MINGW=C:\mingw-w64
set WX=C:\wxWidgets

set PATH=%MINGW%\bin;%PATH%

rem --- CHECK VERSIONS -------------------------------------------------

echo Using compiler:
g++ --version
echo.

rem --- BUILD WXWIDGETS (CORE + COMMON LIBS) ---------------------------

cd /d "%WX%\build\msw"

mingw32-make -f makefile.gcc ^
  SHARED=0 ^
  BUILD=release ^
  UNICODE=1 ^
  MONOLITHIC=0 ^
  USE_OPENGL=1 ^
  USE_XRC=1 ^
  USE_AUI=1 ^
  USE_RIBBON=1 ^
  USE_STC=1 ^
  USE_MEDIA=1 ^
  USE_PROPGRID=1 ^
  USE_RICHTEXT=1 ^
  USE_DEBUGREPORT=1 ^
  USE_GLCANVAS=1 ^
  USE_LIBPNG=1 ^
  USE_LIBJPEG=1 ^
  USE_ZLIB=1 ^
  USE_EXPAT=1 ^
  USE_REGEX=1 ^
  USE_TIFF=0 ^
  USE_WEBVIEW=0 ^
  USE_WEBVIEW_EDGE=0

if %ERRORLEVEL% NEQ 0 (
  echo.
  echo *** wxWidgets build failed ***
  exit /b 1
)

echo.
echo wxWidgets build completed successfully.
echo Libraries are in: %WX%\lib\gcc_lib\mswu
echo.

endlocal
