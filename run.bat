@echo off
rem Forditas, majd a jatek inditasa VICE-ban a lemezkeprol (autostart).
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az emulator helye: masik gepen eleg itt atirni ---
set VICE=C:\GTK3VICE-3.10-win64\bin\x64sc.exe

call "%~dp0build.bat"
if errorlevel 1 exit /b 1

rem a "start" miatt a konzol nem var a VICE bezarasara
start "" "%VICE%" -VICIIfull -autostart "%~dp0build\rogue.d64"
