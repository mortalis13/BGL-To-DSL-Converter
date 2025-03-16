@echo off
path=c:\x86_64-14.2.0-release-win32-seh-msvcrt-rt_v12-rev1\mingw64\bin

set app_name=bgl_to_dsl
set dict_path=%1

if exist %app_name%.exe (
  del %app_name%.exe
)


if "%1"=="c" (
  mingw32-make clean
  goto:eof
)

echo == Building App ==
mingw32-make

if not "%1"=="b" (
if exist %app_name%.exe (
  echo.
  echo == Calling App ==
  echo '%app_name%.exe %dict_path%'
  echo.

  %app_name%.exe "%dict_path%"
)
)
