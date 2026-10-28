@echo off
rem Forditas, majd a jatek inditasa VICE-ban a lemezkeprol (autostart).
rem "run.bat dev": ugyanez a fejlesztoi builddel
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az emulator helye: masik gepen eleg itt atirni ---
set VICE=C:\GTK3VICE-3.10-win64\bin\x64sc.exe

set NAME=rogue
if /i "%~1"=="dev" set NAME=rogue_dev

call "%~dp0build.bat" %1
if errorlevel 1 exit /b 1

rem a "start" miatt a konzol nem var a VICE bezarasara
start "" "%VICE%" -VICIIfull -autostart "%~dp0build\%NAME%.d64"
