@echo off
rem A tesztprogramok forditasa (build.bat test <nev>) es futtatasa, programonkent a szabad hellyel:
rem   "test.bat": mind, egymas utan
rem   "test.bat core": csak a felsoroltak (tobb is lehet, pl. "test.bat core monsters")
rem   "test.bat skip level": mind, a felsoroltak futtatasa nelkul (tobb is lehet); ezek csak lefordulnak
rem   "test.bat help": a hasznalat es a lehetseges nevek (rossz argumentumnal is ez jelenik meg)
rem A tesztprogramok a src\dev\main_test_*.c fajlok, igy az uj program magatol bekerul.
rem A nem interaktiv programok VICE-ben, warp modban, a -debugcart kapcsoloval futnak: a tesztprogram vegen kiirt eredmeny
rem a VICE kilepesi kodja (0 = minden teszt sikerult); a kilepeskori kepernyo build\test_<nev>.png, a VICE naploja build\test_<nev>.log.
rem Az interaktiv programok (INTERACTIVE_PROGRAMS) csak akkor nyilnak meg VICE-ben, ha nevvel kerjuk oket; kulonben csak lefordulnak.
rem A vegen azoknak a listaja, amelyek nem fordultak le, vagy valamelyik tesztjuk nem sikerult, es ilyenkor hibakoddal lep ki;
rem kulon azoke is, amelyeknel a build.bat a dinamikus veremre figyelmeztetett (ez nem hiba).
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az emulator helye: masik gepen eleg itt atirni (a run.bat-ban is) ---
set VICE=C:\GTK3VICE-3.10-win64\bin\x64sc.exe

rem --- az interaktiv tesztprogramok ---
set INTERACTIVE_PROGRAMS= screens

rem ennyi ciklus utan a VICE leall, ha a tesztprogram nem ert veget (pl. egy DEV_CHECK megallitotta); a leghosszabb, a szint-onteszt
rem (level) 2026-11-27-en kb. 5,7 milliard ciklus volt, a hatar kb. fele
set CYCLE_LIMIT=12000000000

setlocal
rem a "call :build_and_run" alatt a %%0 a cimke, ezert a mappa elore elmentve
set ROOT=%~dp0
pushd "%ROOT%"

rem az argumentumok: semmi (mind), nevek (csak ezek), vagy "skip" es nevek (mind, ezek futtatasa nelkul)
set "SELECTED_PROGRAMS= "
set "SKIP_PROGRAMS= "
set SKIP_MODE=
if /i "%~1"=="help" (
    call :put_help
    popd
    exit /b 0
)
if /i "%~1"=="skip" (
    set SKIP_MODE=1
    shift
    if "%~2"=="" goto usage
)
:next_named_program
if "%~1"=="" goto arguments_done
if not exist "src\dev\main_test_%~1.c" (
    echo HIBA: nincs ilyen tesztprogram: "%~1" ^(src\dev\main_test_%~1.c^)
    echo.
    call :put_help
    popd
    exit /b 1
)
if defined SKIP_MODE (
    set "SKIP_PROGRAMS=%SKIP_PROGRAMS%%~1 "
) else (
    set "SELECTED_PROGRAMS=%SELECTED_PROGRAMS%%~1 "
)
shift
goto next_named_program
:arguments_done

set PROGRAM_COUNT=0
set FAILED_PROGRAMS=
set STACK_WARNED_PROGRAMS=
for %%f in (src\dev\main_test_*.c) do call :build_and_run %%~nf

echo.
if defined STACK_WARNED_PROGRAMS echo FIGYELMEZTETES: keves a dinamikus verem:%STACK_WARNED_PROGRAMS%
if defined FAILED_PROGRAMS (
    echo HIBA:%FAILED_PROGRAMS%
    popd
    exit /b 1
)
echo Mind a %PROGRAM_COUNT% tesztprogram rendben.
popd
exit /b 0

:usage
call :put_help
popd
exit /b 1

:put_help
echo Hasznalat:
echo   test.bat                 mind: forditas, a nem interaktivak futtatasa
echo   test.bat ^<nev^> ...       csak a felsoroltak
echo   test.bat skip ^<nev^> ...  mind, a felsoroltak csak lefordulnak
echo   test.bat help            ez a sugo
echo Interaktiv (csak nevvel kerve nyilik meg VICE-ben):%INTERACTIVE_PROGRAMS%
call :put_program_list
goto :eof

rem a tesztprogramok neve a src\dev\main_test_*.c fajlokbol
:put_program_list
set PROGRAM_LIST=
for %%f in (src\dev\main_test_*.c) do call :add_to_program_list %%~nf
echo Tesztprogramok:%PROGRAM_LIST%
goto :eof

:add_to_program_list
set PROGRAM=%1
set PROGRAM_LIST=%PROGRAM_LIST% %PROGRAM:main_test_=%
goto :eof

rem igaz (errorlevel 0), ha a %2 nev szerepel a %1 listaban (szokozzel kezdodo es vegzodo, szokozzel elvalasztott nevek)
:is_in_list
echo %~1 | findstr /i /c:" %~2 " >nul
goto :eof

rem egy tesztprogram forditasa es futtatasa; a parameter a main fajl neve kiterjesztes nelkul (pl. main_test_core)
:build_and_run
set PROGRAM=%1
set PROGRAM=%PROGRAM:main_test_=%

rem nevek megadasakor a tobbi kimarad, forditas nelkul is
set NAMED=
if not "%SELECTED_PROGRAMS%"==" " (
    call :is_in_list "%SELECTED_PROGRAMS%" %PROGRAM%
    if errorlevel 1 goto :eof
    set NAMED=1
)

set /a PROGRAM_COUNT+=1
echo.
echo === test_%PROGRAM%
call "%ROOT%build.bat" test %PROGRAM%
if errorlevel 1 (
    set FAILED_PROGRAMS=%FAILED_PROGRAMS% test_%PROGRAM%[forditas]
    goto :eof
)
rem a build.bat figyelmeztetese a dinamikus veremre: nem hiba, de a vegen is kiirodik
if defined STACK_WARNING set STACK_WARNED_PROGRAMS=%STACK_WARNED_PROGRAMS% test_%PROGRAM%

call :is_in_list "%SKIP_PROGRAMS%" %PROGRAM%
if not errorlevel 1 (
    echo Kihagyva ^(skip^), nem fut.
    goto :eof
)

call :is_in_list "%INTERACTIVE_PROGRAMS% " %PROGRAM%
if not errorlevel 1 (
    if defined NAMED (
        echo Interaktiv, megnyilik VICE-ben.
        rem a "start" miatt nem var a VICE bezarasara; a sajat VICE-beallitasokkal
        start "" "%VICE%" -autostart "%ROOT%build\test_%PROGRAM%.d64"
    ) else (
        echo Interaktiv, csak nevvel kerve nyilik meg ^(test.bat %PROGRAM%^).
    )
    goto :eof
)

echo Fut VICE-ben...
if exist "build\test_%PROGRAM%.png" del "build\test_%PROGRAM%.png"
rem a batch megvarja, amig a VICE kilep; a kilepesi kod az errorlevel: minden nem 0 kod hiba, a negativ is (a kivulrol leallitott
rem VICE -1-gyel lep ki, azt az "if errorlevel 1" nem venne eszre)
rem a VICE a naplojat kulonben a konzolra irna, az atiranyitast megkerulve: ezert -no-redirect-streams, +logtostdout es a naplo fajlba
"%VICE%" -default -sounddev dummy -warp -debugcart -limitcycles %CYCLE_LIMIT% -no-redirect-streams +logtostdout -logfile "build\test_%PROGRAM%.log" -exitscreenshot "build\test_%PROGRAM%.png" -autostart "build\test_%PROGRAM%.d64" >nul 2>&1
if %errorlevel% neq 0 (
    echo HIBA: valamelyik teszt nem sikerult, vagy a program nem ert veget; a kepernyo: build\test_%PROGRAM%.png, a VICE naploja: build\test_%PROGRAM%.log
    set FAILED_PROGRAMS=%FAILED_PROGRAMS% test_%PROGRAM%[teszt]
    goto :eof
)
echo Minden teszt sikerult.
goto :eof
