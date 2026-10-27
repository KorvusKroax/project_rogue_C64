@echo off
rem A jatek forditasa: build\rogue.prg es build\rogue.d64
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az eszkozok helye: masik gepen eleg itt atirni ---
set OSCAR64=C:\oscar64\bin\oscar64.exe

rem --- a jatek forrasfajljai ---
set SOURCES=src\main.c src\state.c src\c64\platform.c

pushd "%~dp0"

if not exist build mkdir build

"%OSCAR64%" -O2 "-o=build\rogue.prg" "-d64=build\rogue.d64" %SOURCES%
if errorlevel 1 goto fail

for /f "tokens=1,3" %%a in ('findstr /c:"HEAP, heap" build\rogue.map') do set /a FREE=0x%%b-0x%%a
echo Szabad hely: %FREE% bajt
echo Kesz: build\rogue.prg, build\rogue.d64
popd
exit /b 0

:fail
echo HIBA: a forditas nem sikerult
popd
exit /b 1
