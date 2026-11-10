@echo off
rem Forditas, majd a jatek inditasa VICE-ban a lemezkeprol (autostart).
rem "run.bat dev": ugyanez a fejlesztoi builddel
rem "run.bat dev 12345": a fejlesztoi builddel, rogzitett seeddel
rem "full" barhol az argumentumok kozott: teljes kepernyon (pl. "run.bat full", "run.bat dev full 12345")
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az emulator helye: masik gepen eleg itt atirni ---
set VICE=C:\GTK3VICE-3.10-win64\bin\x64sc.exe

rem a shift a %%0-t is lepteti, ezert a mappa elore elmentve
set ROOT=%~dp0

rem az argumentumok: a "full" csak a VICE-nek szol, a tobbi sorrendben megy a build.bat-nak
set NAME=rogue
set FULLSCREEN=+VICIIfull
set BUILD_ARGS=
:next_argument
if "%~1"=="" goto arguments_done
if /i "%~1"=="full" (
    set FULLSCREEN=-VICIIfull
) else (
    if /i "%~1"=="dev" set NAME=rogue_dev
    set BUILD_ARGS=%BUILD_ARGS% %1
)
shift
goto next_argument
:arguments_done

call "%ROOT%build.bat"%BUILD_ARGS%
if errorlevel 1 exit /b 1

rem a "start" miatt a konzol nem var a VICE bezarasara
start "" "%VICE%" %FULLSCREEN% -autostart "%ROOT%build\%NAME%.d64"
