# project_rogue_C64

A *Rogue: Exploring the Dungeons of Doom* 5.4.4-es változatának újraírása **Commodore 64-re**.

<!-- TODO: cover kép, pl. ![cover](docs/img/cover.png) -->

## Build

<!-- TODO: eszközlánc (M00-01), fordítási és futtatási lépések (M00-02) -->

C fordítás:

- llvm-mos: `C:\llvm-mos\bin\mos-c64-clang.bat -Os -o build\demo_llvm.prg src\demo.c`
- oscar64: `C:\oscar64\bin\oscar64.exe -O2 "-o=build\demo_oscar.prg" src\demo.c`

## Dokumentáció

- [docs/spec/](docs/spec/) – a C64-es változat specifikációja: a Rogue 5.4.4 minden szabálya, képlete, táblája és üzenete, a C64 képernyőjére, billentyűzetére, memóriájára és lemezkezelésére átdolgozva. A platform miatti eltérések (E1–E17) és az eredeti hibáinak javításai (J1–J17) jegyzéke: [0.3](docs/spec/00_attekintes.md#03-eltérések-és-javítások-jegyzéke); a hardverközeli részletek: [10. Platform](docs/spec/10_platform.md).
- [docs/issues/](docs/issues/) – a megvalósítás mérföldkövenként (M0–M12), issue-nként tartalommal és „Kész, ha” feltételekkel.
