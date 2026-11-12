@echo off
rem A ket fordito osszevetese a szintgeneralon: a keretprogram (src\dev\compare.c) es a szintgeneralo fajlok
rem leforditasa mindket forditoval, -O2 es -Os mellett: build\compare_oscar64-O2.prg, build\compare_oscar64-Os.prg,
rem build\compare_llvm-mos-O2.prg, build\compare_llvm-mos-Os.prg
rem Mindegyik melle az ures keretprogram is lefordul (..._empty.prg); a ketto meretenek kulonbsege a szintgeneralas
rem kodja es allando adata, ezt kiirja.
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az eszkozok helye: masik gepen eleg itt atirni ---
set OSCAR64=C:\oscar64\bin\oscar64.exe
set LLVM_MOS=C:\llvm-mos\bin\mos-c64-clang.bat

rem az llvm-mos -O2 a gyari zero page kerettel (110 bajt) nem linkelheto, mert a zero page tulcsordul; kisebb kerettel igen
set LLVM_MOS_O2_FLAGS=-mlto-zp=32

rem --- a keretprogram es a szintgeneralo fajlok, valtoztatas nelkul ---
set SOURCES=src\dev\compare.c src\rooms.c src\corridors.c src\map.c src\rng.c src\level.c src\state.c

rem --- az ures keretprogram: a szintgeneralo fajlok helyett csonkok ---
set EMPTY_SOURCES=src\dev\compare.c src\dev\compare_empty.c src\state.c

pushd "%~dp0"

if not exist build mkdir build

call :build_oscar64 -O2
if errorlevel 1 goto fail
call :build_oscar64 -Os
if errorlevel 1 goto fail
call :build_llvm_mos -O2
if errorlevel 1 goto fail
call :build_llvm_mos -Os
if errorlevel 1 goto fail

popd
exit /b 0

:fail
echo HIBA: a forditas nem sikerult
popd
exit /b 1

rem --- egy beallitas: a keretprogram es az ures keretprogram, majd a meretek ---
:build_oscar64
"%OSCAR64%" %1 "-o=build\compare_oscar64%1.prg" %SOURCES%
if errorlevel 1 exit /b 1
"%OSCAR64%" %1 "-o=build\compare_oscar64%1_empty.prg" %EMPTY_SOURCES%
if errorlevel 1 exit /b 1
call :report oscar64%1
exit /b 0

:build_llvm_mos
set LLVM_MOS_EXTRA_FLAGS=
if "%1"=="-O2" set LLVM_MOS_EXTRA_FLAGS=%LLVM_MOS_O2_FLAGS%
call "%LLVM_MOS%" %1 %LLVM_MOS_EXTRA_FLAGS% -o build\compare_llvm-mos%1.prg %SOURCES%
if errorlevel 1 exit /b 1
call "%LLVM_MOS%" %1 %LLVM_MOS_EXTRA_FLAGS% -o build\compare_llvm-mos%1_empty.prg %EMPTY_SOURCES%
if errorlevel 1 exit /b 1
call :report llvm-mos%1
exit /b 0

rem a PRG elejen a 2 bajtos betoltesi cim mindkettoben ott van, a kulonbsegben kiesik
:report
for %%F in ("build\compare_%1.prg") do set FULL_SIZE=%%~zF
for %%F in ("build\compare_%1_empty.prg") do set EMPTY_SIZE=%%~zF
set /a LEVEL_SIZE=FULL_SIZE-EMPTY_SIZE
echo %1: %FULL_SIZE% - %EMPTY_SIZE% = %LEVEL_SIZE% bajt (build\compare_%1.prg)
exit /b 0
