@echo off
rem A jatek forditasa: build\rogue.prg es build\rogue.d64
rem "build.bat dev": fejlesztoi build (DEVMODE), build\rogue_dev.prg es build\rogue_dev.d64
rem "build.bat dev 12345": ugyanez rogzitett seeddel (DEV_SEED)
rem "build.bat test core": a src\dev\main_test_core.c tesztprogram, build\test_core.prg es build\test_core.d64
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az eszkozok helye: masik gepen eleg itt atirni ---
set OSCAR64=C:\oscar64\bin\oscar64.exe

rem --- a karakterkeszlet: src\platform\charset_custom.c (sajat) vagy src\platform\charset_rom.c (gyari, a ROM-bol) ---
set CHARSET=src\platform\charset_rom.c

rem --- a jatek forrasfajljai, a main.c nelkul (a tesztprogramoknak sajat main-juk van) ---
set GAME_SOURCES=src\game.c src\texts.c src\state.c src\platform\platform.c src\platform\disk.c src\platform\screen.c src\platform\key.c src\platform\key_codes.c %CHARSET% src\map.c src\view.c src\message.c src\input.c src\status.c src\player.c src\explore.c src\move.c src\rooms.c src\corridors.c src\level.c src\rng.c src\num.c src\monsters.c src\chase.c src\combat.c src\items.c src\item_names.c src\pack.c src\equipment.c src\text_list.c src\discoveries.c

rem --- a fejlesztoi build fajljai; a tesztprogramokba is bekerulnek ---
set DEV_SOURCES=src\dev\dev.c src\dev\dev_commands.c src\dev\start_screen.c src\dev\checks.c

rem --- a tesztek; mindegyik tesztprogram az egeszet kapja, a nem hivott reszeket az Oscar64 kihagyja ---
set TEST_SOURCES=src\dev\test_runner.c src\dev\test_rng.c src\dev\test_num.c src\dev\test_key.c src\dev\test_view.c src\dev\test_message.c src\dev\test_map.c src\dev\test_explore.c src\dev\test_chase.c src\dev\test_level.c src\dev\test_monsters.c src\dev\test_combat.c src\dev\test_items.c src\dev\test_item_names.c src\dev\test_pack.c src\dev\test_equipment.c src\dev\test_text_list.c src\dev\test_screens.c src\dev\test_platform.c

rem --- kiadasi, fejlesztoi vagy teszt build ---
set NAME=rogue
set DEFS=
set SOURCES=src\main.c %GAME_SOURCES%
if /i "%~1"=="dev" (
    set NAME=rogue_dev
    set DEFS=-dDEVMODE
    set SOURCES=src\main.c %GAME_SOURCES% %DEV_SOURCES%
)
rem "build.bat dev 12345": fejlesztoi build rogzitett seeddel
if /i "%~1"=="dev" if not "%~2"=="" set DEFS=%DEFS% -dDEV_SEED=%~2UL
if /i "%~1"=="test" (
    set NAME=test_%~2
    set DEFS=-dDEVMODE
    set SOURCES=src\dev\main_test_%~2.c %GAME_SOURCES% %DEV_SOURCES% %TEST_SOURCES%
)

pushd "%~dp0"

if /i "%~1"=="test" if not exist "src\dev\main_test_%~2.c" (
    echo HIBA: nincs ilyen tesztprogram: "%~2" ^(src\dev\main_test_%~2.c^)
    goto fail_no_message
)

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
:fail_no_message
popd
exit /b 1
