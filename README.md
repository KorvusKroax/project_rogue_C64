# project_rogue_C64

A *Rogue: Exploring the Dungeons of Doom* 5.4.4-es változatának újraírása **Commodore 64-re**.

<!-- TODO: cover kép, pl. ![cover](docs/img/cover.png) -->

## Build

### A játék fordítása és indítása

Szükséges: **Oscar64** (`C:\oscar64`) és **VICE** (`C:\GTK3VICE-3.10-win64`). Ha máshol vannak, a `build.bat` (`OSCAR64`) és a `run.bat` (`VICE`) elején kell átírni az utat.

| parancs | mit csinál |
|---|---|
| `build.bat` | lefordítja a játékot: `build\rogue.prg` és `build\rogue.d64` (a lemezen `rogue` néven) |
| `run.bat` | lefordítja, majd elindítja VICE-ban a lemezképről (autostart) |
| `build.bat dev` | a fejlesztői build: `build\rogue_dev.prg` és `build\rogue_dev.d64` |
| `build.bat dev 12345` | a fejlesztői build rögzített seeddel (itt 12345) |
| `run.bat dev`, `run.bat dev 12345` | ugyanez, és elindítja VICE-ban |

Mindegyik bárhonnan indítható (PowerShellben `.\build.bat`, `.\run.bat`). A C64-en (vagy VICE-ban a lemezkép csatolása után) kézzel: `LOAD"ROGUE",8` vagy `LOAD"ROGUE",8,1`, majd `RUN`.

### Fejlesztői build

A `dev` paraméterrel a fordító a `DEVMODE` makróval fordít ([M00-04](https://github.com/KorvusKroax/project_rogue_C64/issues/4), [10.9](docs/spec/10_platform.md#109-fejlesztői-build-és-determinizmus)). A kiadási buildbe ezekből semmi nem kerül:
- **`DEV_CHECK(feltétel, "üzenet")`** (`src/dev.h`): hamis feltételnél kiírja az üzenetet és a forrásbeli helyet (fájl:sor), majd visszatér a BASIC-be. A feltételnek nem lehet mellékhatása, mert a kiadási buildben ki sem értékelődik.
- **Öntesztek** (`src/dev.c`, a `dev_tests` lista): induláskor lefutnak, az eredményük a képernyőre kerül (`OK`, `1000/1000`, hibánál `FAIL AT <sorszám>`).
- **Seed:** induláskor kiíródik; a második paraméterrel (`build.bat dev 12345`) rögzíthető, így egy hiba ugyanazzal a seeddel újra előidézhető.

A hosszú öntesztek (pl. sok ezer szint generálása) VICE-ben gyorsított módban futtathatók: **Alt+W** ki-be kapcsolja a warp módot (vagy indításkor a `-warp` kapcsoló).

Projektszerkezet:
```
src/          a játék forrása
src/demo/     az M00-01 próbaprogramja (mindkét fordítóval)
build/        a fordítás kimenete (nincs a repóban)
build.bat     fordítás
run.bat       fordítás + indítás VICE-ban
```

### Eszközlánc

**Oscar64 1.32.273** (`C:\oscar64\bin\oscar64.exe`), C99. Döntés: [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1).

A két jelölt (Oscar64 és llvm-mos) ugyanazzal a próbaprogrammal ([src/demo](src/demo/)) lett kipróbálva: RN/rnd, szobarajzolás a videomemóriába, assemblyben írt RN-rutin C-ből hívva, több forrásfájlos fordítás, a BASIC ROM kilapozása és az `$A000–$BFFF` RAM használata. Mindkettő ugyanazt az RN-sorozatot adta. Az Oscar64 mellett szólt:
- kifejezetten C64-re készült, a C64-es teendőkre saját könyvtára van (pl. `conio.h`, `c64/memmap.h`);
- az assembly közvetlenül a C-fájlban, egyszerű szintaxissal írható (`__asm { … }`);
- egyetlen futtatható fájl, a beépített emulátorával (`-e`) VICE nélkül is gyorsan kipróbálható a program;
- várhatóan kisebb kódot ad, ami a játék méreténél a legfontosabb szempont.

A tartalék az **llvm-mos**. A választás visszafordítható marad, mert a játéklogika szabványos C, fordítóspecifikus dolog csak a platformfájlokba kerül. A kódméret és a sebesség ellenőrzése az M03 végén, a valódi szintgenerálón történik ([M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16)).

### A demó fordítása

A demó mindkét fordítóval: `src\demo\build.bat` (bárhonnan indítható).

Kézzel, a projekt gyökeréből:

- oscar64: `C:\oscar64\bin\oscar64.exe -O2 "-o=build\demo_oscar.prg" src\demo\main.c src\demo\rng.c src\demo\room.c src\demo\oscar64\platform.c`
- llvm-mos: `C:\llvm-mos\bin\mos-c64-clang.bat -Os -o build\demo_llvm.prg src\demo\main.c src\demo\rng.c src\demo\room.c src\demo\llvm-mos\platform.c src\demo\llvm-mos\rn.s`

PowerShellben az `-o=…` kapcsolót idézőjelbe kell tenni (`"-o=build\demo_oscar.prg"`), különben a PowerShell a pontnál kettévágja.

## Dokumentáció

- [docs/spec/](docs/spec/) – a C64-es változat specifikációja: a Rogue 5.4.4 minden szabálya, képlete, táblája és üzenete, a C64 képernyőjére, billentyűzetére, memóriájára és lemezkezelésére átdolgozva. A platform miatti eltérések (E1–E17) és az eredeti hibáinak javításai (J1–J17) jegyzéke: [0.3](docs/spec/00_attekintes.md#03-eltérések-és-javítások-jegyzéke); a hardverközeli részletek: [10. Platform](docs/spec/10_platform.md).
- [Mérföldkövek](https://github.com/KorvusKroax/project_rogue_C64/milestones) és [issue-k](https://github.com/KorvusKroax/project_rogue_C64/issues) – a megvalósítás mérföldkövenként (M0–M12), issue-nként tartalommal és „Kész, ha” feltételekkel (áttekintés lent).

## Mérföldkövek és issue-k

Az issue-k GitHubon vannak, mindegyikben: „Függ” (előfeltételek), „Spec” (a vonatkozó fejezetek), „Tartalom” és „Kész, ha” (elfogadási feltételek). Az M4-től kezdve minden mérföldkő végén a program elindul, és az addig kész rész kipróbálható. A belső ellenőrzések és az öntesztek a fejlesztői buildben futnak (M00-04).

| mérföldkő | issue-k |
|---|---|
| **M0 – Eszközlánc és platformalap** | [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1) eszközlánc kiválasztása · [M00-02](https://github.com/KorvusKroax/project_rogue_C64/issues/2) build és indítás · [M00-03](https://github.com/KorvusKroax/project_rogue_C64/issues/3) memóriatérkép, platformréteg · [M00-04](https://github.com/KorvusKroax/project_rogue_C64/issues/4) fejlesztői build |
| **M1 – Alapok** | [M01-01](https://github.com/KorvusKroax/project_rogue_C64/issues/5) RNG, aritmetika |
| **M2 – Képernyő és bevitel** | [M02-01](https://github.com/KorvusKroax/project_rogue_C64/issues/9) karakterkészlet · [M02-02](https://github.com/KorvusKroax/project_rogue_C64/issues/10) billentyűzet · [M02-03](https://github.com/KorvusKroax/project_rogue_C64/issues/11) nézőablak · [M02-04](https://github.com/KorvusKroax/project_rogue_C64/issues/12) üzenetsáv · [M02-05](https://github.com/KorvusKroax/project_rogue_C64/issues/13) státuszsor |
| **M3 – Szintgenerálás** | [M03-01](https://github.com/KorvusKroax/project_rogue_C64/issues/14) szobák, labirintus · [M03-02](https://github.com/KorvusKroax/project_rogue_C64/issues/15) folyosók · [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16) elhelyezés · [M03-04](https://github.com/KorvusKroax/project_rogue_C64/issues/17) fejlesztői mód |
| **M4 – Mozgás, látótér, futás** | [M04-01](https://github.com/KorvusKroax/project_rogue_C64/issues/18) főciklus · [M04-02](https://github.com/KorvusKroax/project_rogue_C64/issues/19) mozgás, látótér · [M04-03](https://github.com/KorvusKroax/project_rogue_C64/issues/20) futás · [M04-04](https://github.com/KorvusKroax/project_rogue_C64/issues/21) keresés, lépcső, ismétlés |
| **M5 – Szörnyek és harc** | [M05-01](https://github.com/KorvusKroax/project_rogue_C64/issues/22) generálás · [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23) ébresztés, mozgás · [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) közelharc · [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) speciális támadások |
| **M6 – Tárgyak és csomag** | [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26) generálás · [M06-02](https://github.com/KorvusKroax/project_rogue_C64/issues/27) fegyverek, nevek · [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) csomag, listák · [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29) felszerelés · [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) szörnyek és tárgyak |
| **M7 – Bájitalok és tekercsek** | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) bájitalok, időzítők · [M07-02](https://github.com/KorvusKroax/project_rogue_C64/issues/32) kijelzési hatások · [M07-03](https://github.com/KorvusKroax/project_rogue_C64/issues/33) tekercsek |
| **M8 – Gyűrűk, pálcák, hajítás** | [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) gyűrűk · [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35) hajítás · [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) pálcák, sugarak |
| **M9 – Csapdák, éhség, vándorlók** | [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) csapdák · [M09-02](https://github.com/KorvusKroax/project_rogue_C64/issues/38) éhség · [M09-03](https://github.com/KorvusKroax/project_rogue_C64/issues/39) regeneráció, vándorlók |
| **M10 – Játék vége, lemezkezelés, címképernyő** | [M10-01](https://github.com/KorvusKroax/project_rogue_C64/issues/40) játék vége · [M10-02](https://github.com/KorvusKroax/project_rogue_C64/issues/41) lemezkezelő réteg · [M10-03](https://github.com/KorvusKroax/project_rogue_C64/issues/42) ranglista · [M10-04](https://github.com/KorvusKroax/project_rogue_C64/issues/43) mentés, betöltés · [M10-05](https://github.com/KorvusKroax/project_rogue_C64/issues/44) címképernyő, beállítások |
| **M11 – Véglegesítés** | [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) szövegek, képernyők · [M11-02](https://github.com/KorvusKroax/project_rogue_C64/issues/46) robot, invariánsok · [M11-03](https://github.com/KorvusKroax/project_rogue_C64/issues/47) végigjátszás · [M11-04](https://github.com/KorvusKroax/project_rogue_C64/issues/48) erőforrások, kiadás |
| **M12 – Opcionális kiegészítések** | [M12-01](https://github.com/KorvusKroax/project_rogue_C64/issues/49) színek · [M12-02](https://github.com/KorvusKroax/project_rogue_C64/issues/50) hangok · [M12-03](https://github.com/KorvusKroax/project_rogue_C64/issues/51) botkormány · [M12-04](https://github.com/KorvusKroax/project_rogue_C64/issues/52) gyorsabb lemez |

## A javítások (J1–J17) helye
| javítás | issue |
|---|---|
| J1 éhség és ISRUN | [M09-02](https://github.com/KorvusKroax/project_rogue_C64/issues/38) |
| J2 Wraith | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) |
| J3 `find_dest` | [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23), [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J4 flytrap | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) |
| J5 see invisible | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31), [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) |
| J6 sárkányláng | [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J7 kincses szoba | [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26) |
| J8 „vanishes” üzenet | [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35) |
| J9 magic missile | [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J10 csapda és páncél | [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
| J11 státuszsor Arm | [M02-05](https://github.com/KorvusKroax/project_rogue_C64/issues/13), [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29), [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) |
| J12 Xeroc és repülés | [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35), [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J13 Medusa | [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23) |
| J14 haste self | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
| J15 `f` átlósan | [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) |
| J16 lépcső csapdán | [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16), [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
| J17 elírások | [M02-04](https://github.com/KorvusKroax/project_rogue_C64/issues/12), [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25), [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) |
