@echo off
rem A jatek forditasa: build\rogue.prg es build\rogue.d64
rem "build.bat dev": fejlesztoi build (DEVMODE), build\rogue_dev.prg es build\rogue_dev.d64
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az eszkozok helye: masik gepen eleg itt atirni ---
set OSCAR64=C:\oscar64\bin\oscar64.exe

rem --- a jatek forrasfajljai ---
set SOURCES=src\main.c src\state.c src\platform.c

rem --- kiadasi vagy fejlesztoi build ---
set NAME=rogue
set DEFS=
if /i "%~1"=="dev" (
    set NAME=rogue_dev
    set DEFS=-dDEVMODE
)

pushd "%~dp0"

if not exist build mkdir build

"%OSCAR64%" -O2 %DEFS% "-o=build\%NAME%.prg" "-d64=build\%NAME%.d64" %SOURCES%
if errorlevel 1 goto fail

for /f "tokens=1,3" %%a in ('findstr /c:"HEAP, heap" build\%NAME%.map') do set /a FREE=0x%%b-0x%%a
echo Szabad hely: %FREE% bajt
echo Kesz: build\%NAME%.prg, build\%NAME%.d64
popd
exit /b 0

:fail
echo HIBA: a forditas nem sikerult
popd
exit /b 1
