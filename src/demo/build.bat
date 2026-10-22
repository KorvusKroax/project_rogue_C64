@echo off
rem Demo forditasa mindket forditoval. Barhonnan indithato, mert a projekt gyokerebe lep.
rem (ebben a fajlban csak ASCII karakter legyen, mert a cmd az ekezeteket felreolvassa)

rem %~dp0 = ennek a .bat fajlnak a mappaja (src\demo\), ket szinttel feljebb a projekt gyokere
pushd "%~dp0..\.."

if not exist build mkdir build

echo === Oscar64
C:\oscar64\bin\oscar64.exe -O2 "-o=build\demo_oscar.prg" src\demo\main.c src\demo\rng.c src\demo\room.c src\demo\oscar64\platform.c
if errorlevel 1 goto fail

echo === llvm-mos
call C:\llvm-mos\bin\mos-c64-clang.bat -Os -o build\demo_llvm.prg src\demo\main.c src\demo\rng.c src\demo\room.c src\demo\llvm-mos\platform.c src\demo\llvm-mos\rn.s
if errorlevel 1 goto fail

echo Kesz: build\demo_oscar.prg, build\demo_llvm.prg
popd
exit /b 0

:fail
popd
exit /b 1
