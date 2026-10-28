# 10. Platform: Commodore 64

Ez a fejezet a hardverközeli követelményeket írja le: memória, képernyő, karakterkészlet, billentyűzet, időzítés, lemezkezelés. A játékszabályok (0–9. fejezet) ezektől függetlenek; a program a kettő között egy vékony **platformréteget** használ (képernyő írása, billentyű olvasása, lemezművelet, időzítés), így a hardverközeli kód egy helyen van ([10.9](#109-fejlesztői-build-és-determinizmus)). Ahol egy megoldás csak ajánlás (pl. pontos memóriacímek), ott ez jelölve van; az eszközlánc választása (assembler, C fordító) ezeket módosíthatja.

## 10.1 Célgép
- Commodore 64 vagy 64C, PAL és NTSC; bővítés (REU, gyorstöltő, SuperCPU) nem szükséges, de nem is zavarhat.
- Lemezegység: 1541 / 1571 / 1581 vagy kompatibilis (SD2IEC, emulátor), 8–11. eszközszám.
- A program egyetlen PRG fájl, `LOAD"ROGUE",8,1` (vagy `,8` + `RUN`) után indul, egy BASIC-indítósorral (`SYS`).
- Fejlesztéshez és teszteléshez: VICE (x64sc) emulátor, a kiadás előtt valódi gépen is ki kell próbálni.

## 10.2 Memória
**Konfiguráció:** a BASIC ROM játék közben ki van lapozva (a `$A000–$BFFF` RAM használható). A KERNAL ROM-ra csak a lemezműveletekhez van szükség; a program választhat, hogy állandóan bent hagyja, vagy csak a lemezműveletek idejére lapozza be (ekkor a `$E000–$FFFF` alatti RAM is használható, de az ott lévő adatot lemezművelet közben nem lehet olvasni, és oda kód sem kerülhet, amelyet lemezművelet közben hívunk).

**Megszakítás:** a saját raszter-IRQ kezelő ([10.6](#106-időzítés)) mindkét állapotban fut: KERNAL nélkül a `$FFFE/$FFFF` vektoron át, KERNAL-lal a `$0314/$0315` vektoron át. A RESTORE billentyű NMI-je egy azonnal visszatérő kezelőre mutat.

**Ajánlott memóriatérkép:**

| tartomány | tartalom |
|---|---|
| `$0000–$00FF` | zero page – a KERNAL lemezrutinjai által használt címeket meg kell hagyni, vagy lemezművelet előtt elmenteni |
| `$0100–$01FF` | hardveres verem (256 bájt: rekurzió nincs, [2.5](02_szintgeneralas.md#25-szobák-rajzolása)) |
| `$0200–$03FF` | KERNAL munkaterület |
| `$0400–$07FF` | szabad (az alapértelmezett képernyő áthelyezve): pl. pufferek, bevitel |
| `$0801–$9FFF` | program és állandó adatok (kb. 38 KB) |
| `$A000–$BFFF` | RAM a BASIC alatt: pl. a játékállapot vagy a szövegek (8 KB) |
| `$C000–$C3FF` | képernyőmemória (VIC 3. bank) |
| `$C400–$C7FF` | szabad (1 KB) |
| `$C800–$CFFF` | saját karakterkészlet (2 KB) |
| `$D000–$DFFF` | I/O, színmemória (`$D800`) |
| `$E000–$FFFF` | KERNAL ROM; alatta 8 KB RAM, ha a KERNAL ki van lapozva |

**Játékállapot:** a teljes perzisztens állapot ([1.4.5](01_adatmodell_es_flagek.md#145-térképrétegek)) egyetlen összefüggő blokk (kb. 11,5 KB), mert a mentés és a betöltés ezt egyben írja és olvassa ([10.7](#107-lemezkezelés)). Ami nem kerül bele: a MAP szörnyréteg (betöltéskor újraépül), a videomemória, a pufferek, az állandó táblák és szövegek.

## 10.3 Képernyő és karakterkészlet
- **VIC beállítás:** 3. bank (`$C000–$FFFF`), képernyő `$C000`, karakterkészlet `$C800` (ajánlás). Szöveges mód, 40×25, normál (nem többszínű, nem kiterjesztett háttérszínű) karakterek.
- **Színek:** keret és háttér fekete, minden karakter világosszürke (15); a színmemória induláskor egyszer kitöltődik. (Színes jelek: opcionális kiegészítés, a játékszabályokat nem érinti.)
- **Karakterkészlet** (2 KB, 256 jel): a 32..126. jel az azonos ASCII-kódú karakter (ASCII-hű betűkészlet, kis- és nagybetűkkel, a PETSCII-ből hiányzó `\` `^` `_` `{` `|` `}` `~` jelekkel is); a 128..254. jel az előző fél inverze (bitenkénti negálás). A 0..31. jel szabad (pl. keretrajzolás az *overwrite* listához, vagy üresen). A 127. és 255. jel nem használt.
- **Írás:** a videomemóriába közvetlenül a jel kódja kerül (ASCII, inverznél +128); a KERNAL képernyőrutinjai (CHROUT a képernyőre) nem használhatók.
- **Sor törlése a végéig**, **inverz megjelenítés**, **teljes képernyő törlése**: a platformréteg egyszerű műveletei.
- A görgetéshez ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)) a térképterület (22 × 40 = 880 bájt) újrarajzolása a SCREEN rétegből egy képkockán belül elvégezhető; hardveres finom görgetés nem kell.

## 10.4 Billentyűzet
- **Mátrixolvasás:** a raszter-IRQ minden képkockában végigolvassa a 8×8-as billentyűmátrixot (CIA1 `$DC00` oszlopkiválasztás, `$DC01` sorolvasás). A KERNAL billentyűrutinja (SCNKEY/GETIN) nem használatos.
- **Módosítók:** bal SHIFT, jobb SHIFT, SHIFT LOCK (a bal SHIFT-tel azonos mátrixpont), CTRL, C=. A logikai billentyű a lenyomott nem-módosító billentyűből és a módosítókból áll elő a [4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet) táblázata szerint: egy-egy táblázat adja a billentyű ASCII-kódját módosító nélkül és SHIFT-tel; CTRL + betű = a betű kódja `& $1F`.
- **Leütés:** egy billentyű akkor számít leütésnek, ha az előző olvasáskor nem volt lenyomva. Több egyszerre lenyomott nem-módosító billentyű közül a mátrix-sorrendben első számít; a ghosting (szellemleütés) kezelése nem szükséges.
- **Ismétlés:** a parancsra váró állapotban 16 képkocka nyomva tartás után ismétlődik, utána 4 képkockánként ([4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet)).
- **Puffer:** 8 elemű körpuffer; az IRQ ír bele, a játék olvas belőle. Ürítés: „billentyűpuffer ürítése” és a `flush` opció.
- **RUN/STOP:** ESC-ként viselkedik, nem szakítja meg a programot.
- **Botkormány:** az 1. porti botkormány a billentyűmátrix vonalait zavarja; ha opcionálisan támogatott, a 2. portot kell használni.

## 10.5 Aritmetika
- **RN:** `seed = seed × 11109 + 13849` (mod 2³²): 32×16 bites szorzás + 32 bites összeadás; az eredmény felső 16 bitje az RN ([0.2](00_attekintes.md#02-alapprimitívek)).
- **rnd(n):** 16 bites előjel nélküli maradékos osztás.
- **24 bites** összeadás/kivonás telítéssel (exp, arany, pontszám), **16 bites előjeles** összeadás/kivonás és összehasonlítás (hp, food_left), 8×8 és 16×8 bites szorzás (`dist2`, értékszámítás).
- **Szám kiírása:** előjeles 16 és előjel nélküli 24 bites érték decimálisan, a státuszsor (3.3) és az üzenetek formázásához (`%d`, `%-Nd`, `%Nd`, `%*d`).
- Lebegőpontos számítás nincs.

## 10.6 Időzítés
- **Raszter-IRQ** képkockánként egyszer (PAL 50 Hz, NTSC 60 Hz): billentyűolvasás, képkocka-számláló.
- **Animációs késleltetés:** a dobott tárgy és a lövedék lépésenként 2 képkockát vár, a sugár (7.6) lépésenként 2-t, és a teljes sugár a visszarajzolás előtt még 10 képkockáig látszik. A nézőablakon kívüli lépések nem várnak ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)). `terse` módban a dobás nem animálódik (4.2.5).
- **Seed:** a címképernyőn az `N` lenyomásakor a 32 bites seed bájtjai: CIA1 A időzítő alsó bájtja, felső bájtja, a raszter-sor (`$D012`) és a jiffy-számláló alsó bájtja, mindegyik XOR-olva a jiffy-számláló többi bájtjával ([0.2](00_attekintes.md#02-alapprimitívek)). Ha az eredmény 0, a seed 1.
- A játéklogika soha nem függ az időtől: az időzítés csak a megjelenítést és a seed kezdőértékét érinti.

## 10.7 Lemezkezelés
**Rutinok:** a KERNAL szabványos rutinjai (SETLFS, SETNAM, OPEN, CHKIN/CHKOUT, CHRIN/CHROUT, READST, CLRCHN, CLOSE), a `device` opció egységszámával ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)). Lemezművelet közben a KERNAL be van lapozva.

**Fájlok** (mind SEQ):
| fájl | tartalom |
|---|---|
| a `file` opció neve (alapból `rogue.sav`) | mentett játék |
| `rogue.scr` | ranglista (legfeljebb 10 bejegyzés) |
| `rogue.cfg` | beállítások és a legutóbbi név |

- **Fájlnevek:** ASCII-ból PETSCII-re alakítva: `a`–`z` → `$41–$5A`, `A`–`Z` → `$C1–$DA`, a többi nyomtatható karakter változatlan. A fájlnév nem tartalmazhat `:` `,` `*` `?` `=` `@` karaktert (ezekre a bekérés nem enged).
- **Hibacsatorna:** minden művelet után a 15. csatornáról a `kód,szöveg,sáv,szektor` sor olvasható; 00–19 rendben, egyébként „disk error: <kód>,<szöveg>”. Ha az egység nincs bekapcsolva (READST: `DEVICE NOT PRESENT`), az üzenet „disk error: device not present”.
- **Létezés-ellenőrzés:** a fájl megnyitása olvasásra, majd a hibacsatorna: `00` → létezik, `62` → nem létezik.
- **Felülírás:** a régi fájl törlése a `S0:<név>` paranccsal, majd új fájl írása. Az `@0:` (save-with-replace) parancs **nem** használható, mert az 1541 DOS-ban hibás.
- **Betöltés utáni törlés:** szintén `S0:<név>`.

**Mentésfájl formátuma:**
| eltolás | méret | tartalom |
|---|---|---|
| 0 | 8 | `ROGUE64` + 0 |
| 8 | 1 | formátumverzió – minden olyan változtatásnál nő, amely az állapotblokk elrendezését módosítja |
| 9 | 2 | az állapotblokk hossza (alacsony bájt előbb) |
| 11 | n | az állapotblokk, elrejtve (lent) |
| 11+n | 2 | 16 bites összeg az elrejtés előtti bájtokból |

Elrejtés: `c[i] = b[i] XOR key[i mod len(key)] XOR c[i−1]` (`c[−1] = 0`), a kulcs a `"rogue 5.4.4 c64"` szöveg ASCII-bájtjai. Ha az azonosító, a verzió, a hossz vagy az összeg nem egyezik, a betöltés „Sorry, saved game is out of date.” üzenettel leáll ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)).

**Ranglista-fájl:** bejegyzésenként: pontszám (3 bájt), név (16 bájt, nullával lezárva), ok (1 bájt: killed / quit / winner / killed with amulet), szint (1 bájt), halálok (1 bájt: szörnybetű, vagy kód az arrow / bolt / dart / hypothermia / starvation okokra). Az elején 1 bájt: a bejegyzések száma.

**Beállításfájl:** a [4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer) opciói rögzített sorrendben (logikai opciók bitekként, szövegek nullával lezárva), elején egy formátumverzió-bájttal; ismeretlen verziónál a fájl figyelmen kívül marad.

**Sebesség:** a KERNAL soros átvitele kb. 400 bájt/s, így egy kb. 11,5 KB-os mentés kb. 30 másodperc. Írás közben a képernyőn „saving...”, olvasás közben „restoring...” felirat látszik a 0. sorban (ezek csak állapotjelzések, nem üzenetek).

## 10.8 Erőforrás-keretek
| tétel | becslés / cél |
|---|---|
| kód | 28–34 KB |
| szövegek (üzenetek, nevek, súgó, szótagok) | kb. 9 KB; ha nem fér el, egyszerű szótáras tömörítés |
| állandó táblák (szörnyek, tárgyak, erő, xp) | kb. 1,5 KB |
| játékállapot (perzisztens blokk) | kb. 11,5 KB |
| MAP szörnyréteg (nem perzisztens) | 1760 bájt |
| képernyő + karakterkészlet | 3 KB |
| **szintgenerálás** | legfeljebb 2 másodperc |
| **egy kör** (szörnymozgással, rajzolással, görgetés nélkül) | szokásos esetben 1–2 képkocka |
| **görgetés** | legfeljebb 2 képkocka |

A fejlesztői mód mutatja a szabad memóriát és a táblák foglaltságát ([1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)).

## 10.9 Fejlesztői build és determinizmus
- A játéklogika (0–9. fejezet) csak a platformrétegen át éri el a hardvert, így a hardverközeli kód egy helyen van.
- **Fejlesztői build:** külön fordítási beállítással készül. Benne vannak a fejlesztői mód eszközei (lásd az M03-04 issue-t), a belső ellenőrzések (hamis feltételnél hibaüzenet a forrásbeli hellyel, majd leállás) és az öntesztek (pl. sok szint generálása és ellenőrzése). A kiadott változatban egyik sincs benne. A hosszú öntesztek VICE-ben, gyorsított (warp) módban futnak.
- **Determinizmus:** ugyanazzal a programváltozattal és ugyanazzal a seeddel ugyanaz a szint és ugyanaz a dobássorozat jön létre, és mentés, majd betöltés után a játék ugyanúgy folytatódik. Az RN-t csak a játéklogika hívhatja; a megjelenítés, a görgetés, az animáció és az időzítés soha.
- A cél az eredeti játékszabályok követése, nem az eredeti program dobássorozatának bájtra pontos utánzása: kódváltozás után ugyanaz a seed más játékot adhat.
