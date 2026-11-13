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
- **`DEV_CHECK(feltétel, "üzenet")`** (`src/dev/dev.h`): hamis feltételnél kiírja az üzenetet és a forrásbeli helyet (fájl:sor), majd a program megáll; a képernyő és a memória marad, így a VICE monitorában megnézhető. A feltételnek nem lehet mellékhatása, mert a kiadási buildben ki sem értékelődik.
- **Öntesztek** (`src/dev/`, a teszteket a `dev_tests` lista sorolja fel a `tests.c`-ben): induláskor lefutnak, az eredményük a képernyőre kerül (`OK`, `1000/1000`, hibánál `FAIL AT <sorszám>`).
- **Seed:** induláskor kiíródik; a menüben az `E` billentyűvel kézzel megadható, vagy a második paraméterrel (`build.bat dev 12345`) fordításkor rögzíthető, így egy hiba ugyanazzal a seeddel újra előidézhető.
- **Fejlesztői parancsok** játék közben, az F-billentyűkön (`src/dev/dev_commands.c`; a kiadási buildben az F-billentyűk nem adnak kódot): **F1** új szint ugyanazon a mélységen, **F3** ugrás tetszőleges mélységre (1..255). Egyik sem visz el időt.

A hosszú öntesztek (pl. sok ezer szint generálása) VICE-ben gyorsított módban futtathatók: **Alt+W** ki-be kapcsolja a warp módot (vagy indításkor a `-warp` kapcsoló).

### A két fordító összevetése

A `compare.bat` a szintgeneráló fájlokat (`rooms`, `corridors`, `map`, `rng`, `level`, `state`) változtatás nélkül, egy kis keretprogrammal (`src/dev/compare.c`) mindkét fordítóval, `-O2` és `-Os` mellett is lefordítja: `build\compare_<fordító>-<beállítás>.prg` (pl. `build\compare_llvm-mos-Os.prg`). Mindegyik mellé az üres keretprogram is lefordul (`src/dev/compare_empty.c`, a szintgenerálás helyett csonkokkal), és a két méret különbsége, a szintgenerálás kódja és állandó adata kiíródik. Ehhez az Oscar64 mellett az **llvm-mos** is kell (`C:\llvm-mos`); ha máshol van, a `compare.bat` elején (`LLVM_MOS`) kell átírni az utat. A PRG-k VICE-ben futnak (pl. a fájlt az ablakra húzva): 100 szintet generálnak, és mélységenként kiírják a generálás átlagos és leglassabb idejét képkockában, valamint a szintek összesített ellenőrzőösszegét, amelynek minden változatnál egyeznie kell. Az eredmények: [Eszközlánc](#eszközlánc), a háttér: [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54).

Projektszerkezet:
```
src/          a játék forrása
build/        a fordítás kimenete (nincs a repóban)
build.bat     fordítás
run.bat       fordítás + indítás VICE-ban
compare.bat   a két fordító összevetése a szintgenerálón
```

### Magyar billentyűzet a VICE-ban

A játék a C64 billentyűzetét olvassa, ezért azt kapja, amit a VICE billentyűkiosztása a C64-re küld. Magyar PC-billentyűzeten a VICE gyári kiosztása három jelet rosszul fordít:

| jel | a PC-n | a gyári kiosztással | következmény |
|---|---|---|---|
| `/` | Shift+6 | SHIFT + `/` megy át, vagyis `?` | a `/` parancs helyett a súgó (`?`) jön |
| `=` | Shift+7 | SHIFT + `=` megy át, ennek nincs kódja | az `=` parancs nem érhető el |
| `\|` | AltGr+W | a `£` megy át SHIFT nélkül, vagyis `\` | a `\|` nem gépelhető be |

Ez a VICE beállításán múlik, nem a játékon, ezért a repó nem ad saját kiosztást: így mindenki megtarthatja a saját VICE-beállításait. A pozicionális kiosztásra váltás nem segít, mert a VICE ott is a jel nevét nézi, és a `/` ugyanúgy `?` marad.

**A javítás** három sor a kiosztásfájlban:

1. Másold a `C:\GTK3VICE-3.10-win64\C64\gtk3_sym.vkm` fájlt egy saját helyre, és írd át benne ezt a három sort:

   | eredeti | új |
   |---|---|
   | `slash 6 7 0` | `slash 6 7 16` |
   | `equal 6 5 0` | `equal 6 5 16` |
   | `bar 6 0 8` | `bar 6 0 1` |

   Egy sor felépítése: `jel oszlop sor jelző`. A `16` azt jelenti, hogy a PC-s SHIFT nem jut át a C64-re, ezért a `/` és az `=` SHIFT nélkül érkezik. Az `1` azt jelenti, hogy a VICE hozzáad egy C64-es SHIFT-et, így a `|` a SHIFT + `£` lesz.

2. Zárd be a VICE-t, és a `%APPDATA%\vice\vice.ini` fájl `[C64SC]` szakaszába írd be vagy írd át ezt a két sort:

   ```
   KeymapIndex=2
   KeymapUserSymFile="C:\saját\út\sajat.vkm"
   ```

   Az útnak teljesnek kell lennie, visszaperrel: perjellel a VICE nem találja meg a fájlt. A többi beállításod érintetlen marad.

Egyszerűbb, de kevésbé tartós megoldás a három sort közvetlenül a `gtk3_sym.vkm`-ben átírni: ekkor a `vice.ini`-hez nem kell nyúlni, viszont a VICE újratelepítése visszaállítja a fájlt.

**Ellenőrzés:** `run.bat dev`, két billentyű a `Keyboard matrix` képernyőig. A `Last press` sorban a Shift+6 47-et, a Shift+7 61-et, az AltGr+W 124-et, a Shift+, (`?`) 63-at kell mutasson. SHIFT alatt ez a képernyő szándékosan nem olvas, ezért a szám a SHIFT felengedése után frissül.

### Eszközlánc

**Oscar64 1.32.273** (`C:\oscar64\bin\oscar64.exe`), C99. Döntés: [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1), megerősítve a valódi szintgenerálón: [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54). Az Oscar64 marad, mert az llvm-mos kódja nem kisebb, hanem 1,7–3,3-szor nagyobb, és az Oscar64-gyel a leglassabb szint is csak 44 képkocka (kb. 0,9 s) a 2 másodperces keretből ([Összevetés a szintgenerálón](#összevetés-a-szintgenerálón)).

A két jelölt (Oscar64 és llvm-mos) ugyanazzal a próbaprogrammal lett kipróbálva: RN/rnd, szobarajzolás a videomemóriába, assemblyben írt RN-rutin C-ből hívva, több forrásfájlos fordítás, a BASIC ROM kilapozása és az `$A000–$BFFF` RAM használata. Mindkettő ugyanazt az RN-sorozatot adta. Az Oscar64 mellett szólt:
- kifejezetten C64-re készült, a C64-es teendőkre saját könyvtára van (pl. `conio.h`, `c64/memmap.h`);
- az assembly közvetlenül a C-fájlban, egyszerű szintaxissal írható (`__asm { … }`);
- egyetlen futtatható fájl, a beépített emulátorával (`-e`) VICE nélkül is gyorsan kipróbálható a program;
- várhatóan kisebb kódot ad, ami a játék méreténél a legfontosabb szempont.

A tartalék az **llvm-mos**. A választás visszafordítható marad, mert a játéklogika szabványos C, fordítóspecifikus dolog csak a platformfájlokba kerül. A kódméret és a sebesség ellenőrzése az M03 végén, a valódi szintgenerálón történt ([M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54)), lásd lent.

#### Összevetés a szintgenerálón

Fordítók: **Oscar64 1.32.273** és **llvm-mos** (clang 24.0.0git, `cd91455`). A mérés a `compare.bat`-tal készült ([A két fordító összevetése](#a-két-fordító-összevetése)): a szintgeneráló fájlok változtatás nélkül, 100 szint a szint-önteszt magjaival (`seed = 1 + i`, az 1., 10., 26. és 40. mélység váltakozva). Az idő a CIA2 két összeláncolt időzítőjével, ciklusban mérve, tiltott CIA1-megszakítással, képkockára átszámolva (PAL: 19 656 ciklus). A méret a keretprogram mérete mínusz az üres keretprogramé, vagyis a szintgenerálás kódja és állandó adata.

| beállítás | méret | átlagos idő | leglassabb |
|---|---|---|---|
| Oscar64 `-O2` | **3 814 bájt** | **21,2 képkocka** | **44,2 képkocka** |
| Oscar64 `-Os` | 3 455 bájt | 30,5 képkocka | 57,5 képkocka |
| llvm-mos `-O2` | 12 406 bájt | 31,1 képkocka | 60,0 képkocka |
| llvm-mos `-Os` | 6 600 bájt | 34,7 képkocka | 67,4 képkocka |

Mélységenként (átlag / leglassabb, képkocka):

| mélység | Oscar64 `-O2` | Oscar64 `-Os` | llvm-mos `-O2` | llvm-mos `-Os` |
|---|---|---|---|---|
| 1 | 10,88 / 12,80 | 14,47 / 17,04 | 18,92 / 22,21 | 21,69 / 25,02 |
| 10 | 24,33 / 42,63 | 35,51 / 56,04 | 34,79 / 59,17 | 38,53 / 64,43 |
| 26 | 26,86 / 39,32 | 38,29 / 51,41 | 38,09 / 55,95 | 42,07 / 60,71 |
| 40 | 22,78 / 44,17 | 33,66 / 57,46 | 32,76 / 60,01 | 36,11 / 67,42 |

- Mind a négy változat ugyanazt a 100 szintet adja (összesített ellenőrzőösszeg: `765E36E1`), és mind bőven belül van a 2 másodperces (100 képkockás) kereten ([10.8](docs/spec/10_platform.md#108-erőforrás-keretek)).
- Az Oscar64 mindkét szempontból jobb: már a `-Os` beállítása is kisebb és gyorsabb kódot ad, mint az llvm-mos bármelyike. Az llvm-mos kódja 1,7-szer (`-Os`; `-Oz`-vel 6 309 bájt) vagy 3,3-szor (`-O2`) akkora. Ha ez az arány a teljes játékra is áll, a becsült 28–34 KB kód llvm-mos-szal 48–58 KB lenne, és nem férne el a 46 976 bájtos fő régióban.
- Az Oscar64-en belül a `-Os` 10%-kal kisebb, de kb. 40%-kal lassabb kódot ad, mint az `-O2`.
- Az llvm-mos `-O2` a gyári zero page-kerettel (`-mlto-zp=110`) nem linkelhető (`relocation R_MOS_ADDR8 out of range`), 61 bájtig igen; a `compare.bat` 32-t ad meg.
- A fordítási idő a keretprogramon mindkettőnél 1 másodperc alatti.

**A váltás költsége.** A játéklogika változás nélkül fordul, Oscar64-specifikus rész csak a `src/platform/` alatt és a `build.bat`-ban van:

| Oscar64 | llvm-mos |
|---|---|
| `#pragma region(main, …)`, `#pragma heapsize(0)` | saját linkerszkript: RAM `$0880–$BFFF`, a verem a `$C000` alatt |
| `__asm raster_interrupt_handler { … }`, `__asm { sei }` | külön `.s` fájl vagy modulszintű `asm` (GNU szintaxis), `SEI()` / `CLI()` |
| `__interrupt`, `__forceinline` | `__attribute__((interrupt))`, `__attribute__((always_inline))` |
| `<c64/vic.h>`, `<c64/cia.h>`, `<c64/memmap.h>` (`vic.`, `cia1.`, `mmap_set`, `vic_setmode`) | `<c64.h>` (`VIC.`, `CIA1.`), a `$01`, `$D018`, `$DD00` közvetlen írása |
| `-d64=` | `c1541` a VICE-ből |
| beépített emulátor (`-e`) | nincs |

Ez kb. 250 sornyi platformkód és egy linkerszkript. A szövegek a ROM-ok alá és a képernyőmodul mindkét fordítóval külön memóriatérképet kíván, ez nem a váltás költsége.

#### Oscar64-hibák és megkerülésük

Az Oscar64 1.32.273 eddig talált hibái. A csendes hibák a veszélyesebbek: ezek futáskor rossz eredményt adnak, hibaüzenet nélkül.

**1. Függvénymutatón át hívott függvény 8 bites visszatérési értéke** – futáskor, csendben; VICE-ben ellenőrizve. A hívó rossz helyről olvassa az eredményt, és egy korábbi értéket kap (a hibás teszt sikeresnek látszott). `-O0`-n is; közvetlen hívásnál és egyelemű táblánál nem jön elő.
```c
// rossz: bool vagy uint8_t visszatérés mutatón át
typedef bool (*TestFunction)(uint16_t i);
bool test_RN(uint16_t i);
if (tests[t](i)) { … }              // régi értéket olvas

// jó: int visszatérés
typedef int (*TestFunction)(uint16_t i);
int test_RN(uint16_t i);
```
Hol: `src/dev/tests.h`. Bejelentve: [#468](https://github.com/drmortalwombat/oscar64/issues/468).

**2. Struct-tag inline asm-ben, és a globális változó asm után** – futáskor, csendben; VICE-ben ellenőrizve. Két külön hiba:
- A `változó.tag` forma asm-ben nem dokumentált, a fordító mégis szó nélkül elfogadja, és a tag eltolását elhagyja. A dokumentált forma a `Típus::tag` ([oscar64.md, Inline Assembler](https://github.com/drmortalwombat/oscar64/blob/main/oscar64.md#inline-assembler)), de ez csak C++ fájlban fordul; C-ben fordítási hiba. Bejelentve: [#471](https://github.com/drmortalwombat/oscar64/issues/471).
- Az optimalizáló nem tudja, hogy az asm írt a globális változóba. Ezt a kiadás után a fő ágban javították ([#392](https://github.com/drmortalwombat/oscar64/issues/392), [#413](https://github.com/drmortalwombat/oscar64/issues/413)), az 1.32.273-ban még benne van.
```c
// rossz
__asm { lda game.seed + 2 }         // a game + 2 címét olvassa, nem a seed 2. bájtját
rn_asm();                           // asm-ben írja a game.seed-et
uint32_t s = game.seed;             // a hívás előtti, gyorsítótárazott értéket adhatja

// C++ fájlban jó: a tag eltolása Típus::tag formában (C-ben nem fordul)
__asm { lda game + GameState::seed + 2 }

// jó: a game-et érintő kód C-ben, így a második hiba sem érinti
uint8_t *seed = (uint8_t *)&game.seed;
seed[2] = …;
```
Hol: `src/rng.c` (az RN C-ben gyorsabb is lett: kb. 870 ciklus az asm-es 1062 helyett).

**3. Típuskényszerítés zárójeles makró előtt** – fordításkor, hibaüzenettel („Array expected”, „Struct expected”). A fordító a kényszerítést a `[ ]` vagy a `.` előtt alkalmazza.
```c
#define JIFFY_CLOCK  ((volatile uint8_t *)0x00a0)

// rossz
uint32_t t = (uint32_t)JIFFY_CLOCK[0];
(void)cia1.icr;

// jó: az operandus zárójelben
uint32_t t = (uint32_t)(JIFFY_CLOCK[0]);
(void)(cia1.icr);
```
Hol: `src/platform/platform.c`. Bejelentve: [#469](https://github.com/drmortalwombat/oscar64/issues/469).

**4. Inline-olt hívás tömbindexként összetett értékadásban** – fordításkor, a fordító összeomlik (`-O2`, „Addressing Error”, kilépési kód 30). Csak akkor jön elő, ha a függvényt hívják és a fordító inline-olja, ezért egy még nem hívott modulban rejtve maradhat; a forrásban kell keresni.
```c
// rossz
game.rooms[choose_random_room()].flags |= ROOM_PHANTOM;

// jó: a hívás eredménye előbb helyi változóba
uint8_t room_index = choose_random_room();
game.rooms[room_index].flags |= ROOM_PHANTOM;
```
Hol: `src/rooms.c`. Bejelentve: [#470](https://github.com/drmortalwombat/oscar64/issues/470).

**5. Rövid ciklusú függvény egymás után, más-más állandó argumentummal** – futáskor, csendben; a tesztképernyőn látszott, de nincs feljegyezve, hogy VICE-ben vagy az `-e` emulátorban. `-O2`-n az előző hívás eredményét adja vissza, ha a ciklus 3–4 lépéses (pl. 2×2-es szobarácsnál); 8–9 lépésnél helyes. A `__noinline` és a `#pragma unroll(0)` nem segít.
```c
// rossz
static uint8_t count_rooms_with_flag(uint8_t flag)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < ROOM_COUNT; i++) {      // ROOM_COUNT = 4
        if (game.rooms[i].flags & flag) count++;
    }
    return count;
}
uint8_t phantom_count = count_rooms_with_flag(ROOM_PHANTOM);
uint8_t dark_count = count_rooms_with_flag(ROOM_DARK);   // a phantom_count értékét kaphatja

// jó: egy menetben, több számlálóval
for (uint8_t i = 0; i < ROOM_COUNT; i++) {
    uint8_t flags = game.rooms[i].flags;
    if (flags & ROOM_PHANTOM) phantom_count++;
    if (flags & ROOM_DARK) dark_count++;
}
```
Hol: `src/dev/test_screens.c`.

**6. `uint8_t` számlálós ciklus több `printf`-fel** – futáskor, csendben; eddig csak az `-e` emulátorban jött elő, VICE-ben nincs ellenőrizve. `-O2`-n kimarad egy lépés; `-O0`-n, `uint16_t` számlálóval vagy kevesebb `printf`-fel helyes.
```c
// rossz: 3 helyett 2-szer fut
for (uint8_t d = 1; d < 40; d += 13) {
    printf("%d", d); printf("%d", d); printf("%d", d);
}

// jó
for (uint16_t d = 1; d < 40; d += 13) { … }
```
A játék nem használ `printf`-et; próbaprogramban számít.

**Tárgytalan platformcsapdák.** Ezek nem fordítóhibák voltak, és a BASIC-be visszatérés elhagyásával megszűntek ([10.1](docs/spec/10_platform.md#101-célgép)):
```c
// a BASIC ROM visszalapozása után a $B000 fölötti C-verem helyén ROM látszott
platform_exit();
put_text("bye");                    // összeomlott

// a fordító zero page-e felülírja a BASIC szövegverem-mutatóit ($16–$18)
return 0;                           // a BASIC-ben néhány parancs után ?FORMULA TOO COMPLEX
```

## Dokumentáció

- [docs/spec/](docs/spec/) – a C64-es változat specifikációja: a Rogue 5.4.4 minden szabálya, képlete, táblája és üzenete, a C64 képernyőjére, billentyűzetére, memóriájára és lemezkezelésére átdolgozva. A platform miatti eltérések (E1–E20) és az eredeti hibáinak javításai (J1–J19) jegyzéke: [0.3](docs/spec/00_attekintes.md#03-eltérések-és-javítások-jegyzéke); a hardverközeli részletek: [10. Platform](docs/spec/10_platform.md).
- [Mérföldkövek](https://github.com/KorvusKroax/project_rogue_C64/milestones) és [issue-k](https://github.com/KorvusKroax/project_rogue_C64/issues) – a megvalósítás mérföldkövenként (M0–M12), issue-nként tartalommal és „Kész, ha” feltételekkel (áttekintés lent).

## Mérföldkövek és issue-k

Az issue-k GitHubon vannak, mindegyikben: „Függ” (előfeltételek), „Spec” (a vonatkozó fejezetek), „Tartalom” és „Kész, ha” (elfogadási feltételek). Az M4-től kezdve minden mérföldkő végén a program elindul, és az addig kész rész kipróbálható. A belső ellenőrzések és az öntesztek a fejlesztői buildben futnak (M00-04).

| mérföldkő | issue-k |
|---|---|
| **M0 – Eszközlánc és platformalap** | [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1) eszközlánc kiválasztása · [M00-02](https://github.com/KorvusKroax/project_rogue_C64/issues/2) build és indítás · [M00-03](https://github.com/KorvusKroax/project_rogue_C64/issues/3) memóriatérkép, platformréteg · [M00-04](https://github.com/KorvusKroax/project_rogue_C64/issues/4) fejlesztői build |
| **M1 – Alapok** | [M01-01](https://github.com/KorvusKroax/project_rogue_C64/issues/5) RNG, aritmetika |
| **M2 – Képernyő és bevitel** | [M02-01](https://github.com/KorvusKroax/project_rogue_C64/issues/9) karakterkészlet · [M02-02](https://github.com/KorvusKroax/project_rogue_C64/issues/10) billentyűzet · [M02-03](https://github.com/KorvusKroax/project_rogue_C64/issues/11) nézőablak · [M02-04](https://github.com/KorvusKroax/project_rogue_C64/issues/12) üzenetsáv · [M02-05](https://github.com/KorvusKroax/project_rogue_C64/issues/13) státuszsor |
| **M3 – Szintgenerálás** | [M03-01](https://github.com/KorvusKroax/project_rogue_C64/issues/14) szobák, labirintus · [M03-02](https://github.com/KorvusKroax/project_rogue_C64/issues/15) folyosók · [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16) elhelyezés · [M03-04](https://github.com/KorvusKroax/project_rogue_C64/issues/17) fejlesztői mód · [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54) eszközlánc-ellenőrzés |
| **M4 – Mozgás, látótér, futás** | [M04-01](https://github.com/KorvusKroax/project_rogue_C64/issues/18) főciklus · [M04-02](https://github.com/KorvusKroax/project_rogue_C64/issues/19) mozgás, látótér · [M04-03](https://github.com/KorvusKroax/project_rogue_C64/issues/20) futás · [M04-04](https://github.com/KorvusKroax/project_rogue_C64/issues/21) keresés, lépcső, ismétlés |
| **M5 – Szörnyek és harc** | [M05-01](https://github.com/KorvusKroax/project_rogue_C64/issues/22) generálás · [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23) ébresztés, mozgás · [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) közelharc · [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) speciális támadások |
| **M6 – Tárgyak és csomag** | [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26) generálás · [M06-02](https://github.com/KorvusKroax/project_rogue_C64/issues/27) fegyverek, nevek · [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) csomag, listák · [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29) felszerelés · [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) szörnyek és tárgyak |
| **M7 – Bájitalok és tekercsek** | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) bájitalok, időzítők · [M07-02](https://github.com/KorvusKroax/project_rogue_C64/issues/32) kijelzési hatások · [M07-03](https://github.com/KorvusKroax/project_rogue_C64/issues/33) tekercsek · [M07-04](https://github.com/KorvusKroax/project_rogue_C64/issues/53) szövegkatalógus generálása |
| **M8 – Gyűrűk, pálcák, hajítás** | [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) gyűrűk · [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35) hajítás · [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) pálcák, sugarak |
| **M9 – Csapdák, éhség, vándorlók** | [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) csapdák · [M09-02](https://github.com/KorvusKroax/project_rogue_C64/issues/38) éhség · [M09-03](https://github.com/KorvusKroax/project_rogue_C64/issues/39) regeneráció, vándorlók |
| **M10 – Játék vége, lemezkezelés, címképernyő** | [M10-01](https://github.com/KorvusKroax/project_rogue_C64/issues/40) játék vége · [M10-02](https://github.com/KorvusKroax/project_rogue_C64/issues/41) lemezkezelő réteg · [M10-03](https://github.com/KorvusKroax/project_rogue_C64/issues/42) ranglista · [M10-04](https://github.com/KorvusKroax/project_rogue_C64/issues/43) mentés, betöltés · [M10-05](https://github.com/KorvusKroax/project_rogue_C64/issues/44) címképernyő, beállítások |
| **M11 – Véglegesítés** | [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) szövegek, képernyők · [M11-02](https://github.com/KorvusKroax/project_rogue_C64/issues/46) robot, invariánsok · [M11-03](https://github.com/KorvusKroax/project_rogue_C64/issues/47) végigjátszás · [M11-04](https://github.com/KorvusKroax/project_rogue_C64/issues/48) erőforrások, kiadás |
| **M12 – Opcionális kiegészítések** | [M12-01](https://github.com/KorvusKroax/project_rogue_C64/issues/49) színek · [M12-02](https://github.com/KorvusKroax/project_rogue_C64/issues/50) hangok · [M12-03](https://github.com/KorvusKroax/project_rogue_C64/issues/51) botkormány · [M12-04](https://github.com/KorvusKroax/project_rogue_C64/issues/52) gyorsabb lemez |

## A javítások (J1–J19) helye
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
| J17 elírások | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25), [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) |
| J18 labirintus kijárata | [M03-02](https://github.com/KorvusKroax/project_rogue_C64/issues/15) |
| J19 a szint berendezése mindig véget ér | [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16), [M05-01](https://github.com/KorvusKroax/project_rogue_C64/issues/22), [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26), [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
