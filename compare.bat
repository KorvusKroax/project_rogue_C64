@echo off
rem A ket fordito osszevetese a szintgeneralon: a keretprogram (src\dev\compare.c) es a szintgeneralo fajlok
rem leforditasa mindket forditoval: build\compare_oscar64.prg es build\compare_llvm-mos.prg
rem Barhonnan indithato, mert a sajat mappajaba (a projekt gyokerebe) lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem --- az eszkozok helye: masik gepen eleg itt atirni ---
set OSCAR64=C:\oscar64\bin\oscar64.exe
set LLVM_MOS=C:\llvm-mos\bin\mos-c64-clang.bat

rem --- a keretprogram es a szintgeneralo fajlok, valtoztatas nelkul ---
set SOURCES=src\dev\compare.c src\rooms.c src\corridors.c src\map.c src\rng.c src\level.c src\state.c

pushd "%~dp0"

if not exist build mkdir build

"%OSCAR64%" -O2 "-o=build\compare_oscar64.prg" %SOURCES%
if errorlevel 1 goto fail

call "%LLVM_MOS%" -Os -o build\compare_llvm-mos.prg %SOURCES%
if errorlevel 1 goto fail

echo Kesz: build\compare_oscar64.prg, build\compare_llvm-mos.prg
popd
exit /b 0

:fail
echo HIBA: a forditas nem sikerult
popd
exit /b 1
