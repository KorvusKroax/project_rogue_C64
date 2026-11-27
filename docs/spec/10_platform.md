# 10. Platform: Commodore 64

Ez a fejezet a hardverközeli követelményeket írja le: memória, képernyő, karakterkészlet, billentyűzet, időzítés, lemezkezelés. A játékszabályok (0–9. fejezet) ezektől függetlenek; a program a kettő között egy vékony **platformréteget** használ (képernyő írása, billentyű olvasása, lemezművelet, időzítés), így a hardverközeli kód egy helyen van ([10.9](#109-fejlesztői-build-és-determinizmus)). Ahol egy megoldás csak ajánlás (pl. pontos memóriacímek), ott ez jelölve van; az eszközlánc választása (assembler, C fordító) ezeket módosíthatja.

## 10.1 Célgép
- Commodore 64 vagy 64C, PAL és NTSC; bővítés (REU, gyorstöltő, SuperCPU) nem szükséges, de nem is zavarhat.
- Lemezegység: 1541 / 1571 / 1581 vagy kompatibilis (SD2IEC, emulátor), 8–11. eszközszám.
- A program három PRG fájl:
  - a `rogue` maga a játék;
  - a `texts` a játék szövegei ([10.2](#102-memória)). A fordító ugyanabban a menetben írja ki, mint a játékot, és a nevét a szövegek régiójának nevéből adja, amely nem tartalmazhat pontot, ezért nincs `rogue.` előtagja;
  - a `rogue.screens` a képernyőmodul ([10.10](#1010-képernyőmodul)).

  A játék `LOAD"ROGUE",8,1` (vagy `,8` + `RUN`) után indul, egy BASIC-indítósorral (`SYS`). A szövegeket és a modult a játék tölti be. A kiadás egy lemezkép, amelyen mindhárom fájl rajta van.
- A programból nincs kilépés a BASIC-be: a játék vége után a címképernyő jön ([0.1](00_attekintes.md#01-indítás)), a gépet újraindítással lehet elhagyni. Így a kilépéskor semmit nem kell visszaállítani (megszakításvektorok, ROM-lapozás, a BASIC zero page-e).
- Fejlesztéshez és teszteléshez: VICE (x64sc) emulátor, a kiadás előtt valódi gépen is ki kell próbálni.

## 10.2 Memória
**Konfiguráció:** a BASIC ROM játék közben ki van lapozva, így a `$A000–$BFFF` RAM is használható. A KERNAL ROM és az I/O bent van. Csak egy szöveg kiolvasásának idejére lapozódik ki minden ROM és az I/O is (`$01 = $30`), mert a szövegek a `$D000–$FFF9` alatti RAM-ban vannak (lásd lent).

**Megszakítás:** a saját raszter-IRQ kezelő ([10.6](#106-időzítés)) a `$0314/$0315` vektoron át fut. Szöveg kiolvasása közben a megszakítás tiltva van, így ekkor az IRQ nem fut. A RESTORE billentyű NMI-jét a tiltás nem állítja meg, ezért mindkét helyen egy azonnal visszatérő kezelőre mutat: a KERNAL-lal a `$0318/$0319` vektoron át, kilapozott ROM mellett a RAM-beli `$FFFA/$FFFB` vektoron át. Emiatt a `$FFFA–$FFFF` 6 bájtjára nem kerülhet szöveg.

**Ajánlott memóriatérkép:**

| tartomány | tartalom |
|---|---|
| `$0000–$00FF` | zero page – a KERNAL lemezrutinjai által használt címeket meg kell hagyni, vagy lemezművelet előtt elmenteni |
| `$0100–$01FF` | hardveres verem (256 bájt: rekurzió nincs, [2.5](02_szintgeneralas.md#25-szobák-rajzolása)) |
| `$0200–$03FF` | KERNAL munkaterület |
| `$0400–$07FF` | az RNG bájtonkénti szorzótáblái (4 × 256 bájt, [10.5](#105-aritmetika)); az alapértelmezett képernyő áthelyezve. A lemezre nem kerülnek, és induláskor nem nullázódnak: az `rng_init` tölti fel őket |
| `$0801–$087F` | BASIC-indítósor |
| `$0880–$BFFF` | fő régió (46 976 bájt): kód, állandó adatok, változók, a fordító szoftveres verme (512 bájt, `#pragma stacksize`, lásd lent); a játékállapot-blokk helyére a címképernyő idején a képernyőmodul töltődik ([10.10](#1010-képernyőmodul)) |
| `$C000–$C3FF` | képernyőmemória (VIC 3. bank) |
| `$C400–$C7FF` | pufferek (1 KB, lapozás nélkül elérhető): induláskor a szövegek betöltésének puffere (lásd lent), utána az üzenetek, a nevek, a listasor, a felfedezések listája és a labirintus verme (2026-11-27-én a `$C6E7`-ig, 280 bájt szabad). A lemezre nem kerülnek, és induláskor nem nullázódnak, sőt a betöltés után a szövegfájl utolsó darabja marad itt. Ezért csak olyan puffer kerülhet ide, amely a betöltés közben nem kell, és amelybe olvasás előtt mindig írás történik; az előző üzenetet (CTRL-P) a játék új játéknál és betöltés után üríti |
| `$C800–$CFFF` | saját karakterkészlet (2 KB) |
| `$D000–$DFFF` | I/O, színmemória (`$D800`); alatta RAM: szövegek |
| `$E000–$FFF9` | KERNAL ROM; alatta RAM: szövegek |
| `$FFFA–$FFFF` | KERNAL ROM; alatta RAM: az NMI-, RESET- és IRQ-vektor a kilapozott ROM idejére |

**Verem:** a fordító szoftveres vermének 512 bájtja a régió végén van. A teteje a függvények rögzített helyű keretei a helyi változóiknak, alattuk a dinamikus verem. A `.map` fájl STACK sora a megmaradt dinamikus verem, a keretek a sor vége (`StackEnd`) és a régió vége között vannak. 2026-11-27-én mérve a keretek a fejlesztői buildben 383, a játékban 293 bájtot foglalnak.

**Szövegek:** minden játékszöveg ([1.4.4](01_adatmodell_es_flagek.md#144-szövegek)) a `$D000–$FFF9` alatti RAM-ban van (12 282 bájt; a rezidens szövegek a végén kb. 10,2 KB-ot foglalnak, 2026-11-24-én 4 825 bájtot). A szövegek és a tábláik egy külön szakaszban (`texts`) vannak, amelynek régiója `$D000`-nál kezdődik. A folytatásukra a `$C400–$C7FF` már nem jöhet szóba, mert ott pufferek vannak.
- **Kiolvasás:** a megszakítás tiltása, `$01 = $30`, a szöveg átmásolása a hívó pufferébe a fő régióban, legfeljebb a megadott hosszig, a `$01` visszaállítása, végül a megszakítás régi állapota (tiltott megszakítás mellett hívva tiltva marad). Bármely címről olvas, mert `$01 = $30` mellett a fő régió ugyanúgy látszik, így a hívónak nem kell tudnia, hol a szöveg. Táblaelemnél a mutató kiolvasása és a szöveg másolása egy lapozásban történik; ha a táblaelem címe továbbadódik (pl. üzenet mintájaként vagy helyőrzőjeként), csak a mutató olvasódik ki, és a szöveget a felhasználás helye másolja.
- **Betöltés:** induláskor, az első szövegkiírás előtt, a játékban és minden tesztprogramban, arról az egységről, amelyről a játék betöltődött (8 alatti egységszámnál a 8-asról). A fájl a `$D000`-s betöltési címmel kezdődik. A `LOAD` a `$D000–$DFFF` alá nem írhat, mert ott az I/O látszik, ezért a fájl 256 bájtos darabokban a `$C400`-as pufferbe olvasódik, és minden darab `$01 = $30` mellett másolódik a helyére. A betöltés csak akkor sikeres, ha a betöltési cím `$D000`, és pontosan a szakasz hossza jött, utána a fájl véget ért. Közben a 0. sorban „loading...” látszik, hibánál „insert the game disk and press space”, és szóközre újra ([10.7](#107-lemezkezelés)). A 4,8 KB-os fájl kb. 9–12 másodperc. Szöveg nélküli programban (pl. egy tesztprogramban) a szakasz üres, és a betöltés elmarad.
- **Tömörítés:** még nincs. A szövegkatalógus generálása után bájtpár-kódolással tömörülnek, hogy a ROM-ok alatt kódnak is jusson hely ([10.8](#108-erőforrás-keretek)).

**Játékállapot:** a teljes perzisztens állapot ([1.4.5](01_adatmodell_es_flagek.md#145-térképrétegek)) egyetlen összefüggő blokk (kb. 8,5 KB), mert a mentés és a betöltés ezt egyben írja és olvassa ([10.7](#107-lemezkezelés)). Ami nem kerül bele:
- a beállításblokk, mert a képernyőmodul is használja; a mentés ettől még kiírja ([10.10](#1010-képernyőmodul));
- a videomemória, a pufferek, az állandó táblák és a szövegek.

## 10.3 Képernyő és karakterkészlet
- **VIC beállítás:** 3. bank (`$C000–$FFFF`), képernyő `$C000`, karakterkészlet `$C800` (ajánlás). Szöveges mód, 40×25, normál (nem többszínű, nem kiterjesztett háttérszínű) karakterek.
- **Színek:** keret és háttér fekete, minden karakter világosszürke (15); a színmemória induláskor egyszer kitöltődik. (Színes jelek: opcionális kiegészítés, a játékszabályokat nem érinti.)
- **Karakterkészlet** (2 KB, 256 jel): a 32..126. jel az azonos ASCII-kódú karakter (ASCII-hű betűkészlet, kis- és nagybetűkkel, a PETSCII-ből hiányzó `\` `^` `_` `{` `|` `}` `~` jelekkel is); a 128..254. jel az előző fél inverze (bitenkénti negálás). A 0..31. jel szabad. A 127. és 255. jel nem használt.
- **Írás:** a videomemóriába közvetlenül a jel kódja kerül (ASCII, inverznél +128); a KERNAL képernyőrutinjai (CHROUT a képernyőre) nem használhatók.
- **Sor törlése a végéig**, **inverz megjelenítés**, **teljes képernyő törlése**: a platformréteg egyszerű műveletei.
- A görgetéshez ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)) a térképterület (22 × 40 = 880 bájt) újrarajzolása a SCREEN rétegből egy képkockán belül elvégezhető; hardveres finom görgetés nem kell.

## 10.4 Billentyűzet
- **Mátrixolvasás:** a raszter-IRQ minden képkockában végigolvassa a 8×8-as billentyűmátrixot (CIA1 `$DC00` oszlopkiválasztás, `$DC01` sorolvasás). A KERNAL billentyűrutinja (SCNKEY/GETIN) nem használatos, ezért az IRQ nem a `$EA31`-re ugrik: a jiffy órát az UDTIM (`$FFEA`) lépteti, a visszatérés a `$EA81`-en át történik.
- **Módosítók:** bal SHIFT, jobb SHIFT, SHIFT LOCK (a bal SHIFT-tel azonos mátrixpont), CTRL, C=. A logikai billentyű a lenyomott nem-módosító billentyűből és a módosítókból áll elő a [4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet) táblázata szerint: egy-egy táblázat adja a billentyű ASCII-kódját módosító nélkül és SHIFT-tel; CTRL vagy C= + betű = a betű kódja `& $1F`.
- **Leütés:** egy billentyű akkor számít leütésnek, ha az előző olvasáskor nem volt lenyomva; a még nyomva tartott billentyűk nem takarják el. Egy képkockában legfeljebb egy leütés számít: az új billentyűk közül a mátrix-sorrendben első, a többi a következő képkockákban, így egyik sem vész el. A módosítók és a kód nélküli billentyűk (a függőleges kurzor, a SHIFT + CLR/HOME, bekapcsolás nélkül az F) nem számítanak. Az F-billentyűk csak akkor adnak kódot, ha a fejlesztői kód induláskor bekapcsolja őket ([10.9](#109-fejlesztői-build-és-determinizmus)); a kiadási változatban ez nem történik meg. A ghosting (szellemleütés) kezelése nem szükséges.
- **Ismétlés:** a parancsra váró állapotban 16 képkocka nyomva tartás után ismétlődik, utána 4 képkockánként ([4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet)). A játék csak a parancsra várakozás idejére kapcsolja be, és az ismételt kód csak üres pufferbe kerül.
- **Puffer:** 8 elemű körpuffer, mind a 8 hely használható; az IRQ ír bele, a játék olvas belőle. Tele pufferbe érkező leütés eldobódik. Ürítés: „billentyűpuffer ürítése” és a `flush` opció.
- **RUN/STOP:** ESC-ként viselkedik, nem szakítja meg a programot.
- **Botkormány:** az 1. porti botkormány a billentyűmátrix vonalait zavarja; ha opcionálisan támogatott, a 2. portot kell használni.

## 10.5 Aritmetika
- **RN:** `seed = seed × 11109 + 13849` (mod 2³²): 32×16 bites szorzás + 32 bites összeadás; az eredmény felső 16 bitje az RN ([0.2](00_attekintes.md#02-alapprimitívek)).
  - **Bájtonkénti szorzótáblák:** a seed minden bájtjának a szorzó alsó (`$65`) és felső (`$2B`) bájtjával vett szorzata táblából jön (4 × 256 bájt a fő régión kívül, a `$0400–$07FF` területen, [10.2](#102-memória); induláskor feltöltve, `rng_init`), és a szorzat bájtoszloponként, átvitellel adódik össze, mint az írásbeli szorzásnál; a 3. bájt fölötti rész elveszik. Egy `RN` kb. 170 µs (az Oscar64 általános 32 bites szorzójával kb. 750 µs), bitre ugyanazzal a sorozattal.
  - **Tábla nélkül:** eltolással és összeadással is számolható (`$2B65`: az alsó bájt 0., 2., 5., 6., a felső 0., 1., 3., 5. bitje), kb. 410 µs. A fő régióban ez már nem adna helyet, mert a táblák nem ott vannak, csak a `$0400–$07FF` szabadulna fel.
- **rnd(n):** 16 bites előjel nélküli maradékos osztás.
- **32 bites előjel nélküli** összeadás/kivonás telítéssel (felül 16 777 215-nél, alul 0-nál: exp, arany, pontszám), **16 bites előjeles** összeadás/kivonás és összehasonlítás (hp, food_left), 16×8 bites szorzás (értékszámítás).
- **Táblázatok szorzás helyett:**
  - **négyzettábla:** 0..79 négyzetei (80 × 16 bit = 160 bájt); a `dist2` két kiolvasás és egy összeadás;
  - **sorcím-tábla:** a térképrétegek soronkénti kezdő eltolása, `(y − 1) × 80` (22 × 16 bit = 44 bájt); a mezőindex egy kiolvasás és egy összeadás.
- **8 bites számolás, ahol elég:** koordináták, számlálók, kis értékek; a 16 bites művelet a 6502-n két-háromszor lassabb. Ahol egy érték biztosan elfér 8 biten, ott nem használunk 16 bitet.
- **Szám kiírása:** egyetlen, előjeles 32 bites értéket váró rutin, decimálisan (az arany és a tapasztalat 16 777 215-nél telítődik, így belefér; a 32 bites seedet a fejlesztői menü az utolsó jegyét külön kiírva jeleníti meg, így előjel nélkül, a teljes tartományban), a státuszsor (3.3) és az üzenetek formázásához (`%d`, `%-Nd`, `%Nd`).
- Lebegőpontos számítás nincs.

## 10.6 Időzítés
- **Raszter-IRQ** képkockánként egyszer (PAL 50 Hz, NTSC 60 Hz): billentyűolvasás, képkocka-számláló, jiffy óra.
- **Futás üteme:** futás közben (`jump_to_run_end` nélkül) minden lépés után a játék a következő ütem elejéig vár, így minden lépés ugyanannyi képkockáig tart (3 vagy 4, [10.8](#108-erőforrás-keretek)), akár van szörny a közelben, akár nincs. Ha egy lépés ennél tovább tart (pl. szobába lépés, sok gyorsított vagy repülő szörny), a következő ütemig vár, így az ütem egyszer csúszik. `jump_to_run_end` mellett nincs várakozás, mert a futás közben nincs rajzolás.
- **Animációs késleltetés:** a dobott tárgy és a lövedék lépésenként 2 képkockát vár, a sugár (7.6) lépésenként 2-t, és a teljes sugár a visszarajzolás előtt még 10 képkockáig látszik. A nézőablakon kívüli lépések nem várnak ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).
- **Seed:** a címképernyőn az `N` lenyomásakor a 32 bites seed bájtjai: CIA1 A időzítő alsó bájtja, felső bájtja, a raszter-sor (`$D012`) és a jiffy-számláló alsó bájtja, mindegyik XOR-olva a jiffy-számláló többi bájtjával ([0.2](00_attekintes.md#02-alapprimitívek)). Ha az eredmény 0, a seed 1.
- A játéklogika soha nem függ az időtől: az időzítés csak a megjelenítést és a seed kezdőértékét érinti.

## 10.7 Lemezkezelés
**Rutinok:** a KERNAL szabványos rutinjai (SETLFS, SETNAM, OPEN, CHKIN/CHKOUT, CHRIN/CHROUT, READST, CLRCHN, CLOSE), a `device` opció egységszámával ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)). Lemezművelet közben a KERNAL be van lapozva.

**Fájlok** (a szövegek és a képernyőmodul PRG, a többi SEQ):
| fájl | tartalom |
|---|---|
| `texts` | a játék szövegei ([10.2](#102-memória)) |
| `rogue.screens` | képernyőmodul ([10.10](#1010-képernyőmodul)) |
| a `file` opció neve (alapból `rogue.sav`) | mentett játék |
| `rogue.scr` | ranglista (legfeljebb 10 bejegyzés) |
| `rogue.cfg` | beállítások és a legutóbbi név |

- **Fájlnevek:** ASCII-ból PETSCII-re alakítva: `a`–`z` → `$41–$5A`, `A`–`Z` → `$C1–$DA`, a többi nyomtatható karakter változatlan. A fájlnév nem tartalmazhat `:` `,` `*` `?` `=` `@` karaktert (ezekre a bekérés nem enged).
- **Hibacsatorna:** minden művelet után a 15. csatornáról a `kód,szöveg,sáv,szektor` sor olvasható; 00–19 rendben, egyébként „disk error: <kód>,<szöveg>”. Ha az egység nincs bekapcsolva (READST: `DEVICE NOT PRESENT`), az üzenet „disk error: device not present”.
- **Létezés-ellenőrzés:** a fájl megnyitása olvasásra, majd a hibacsatorna: `00` → létezik, `62` → nem létezik.
- **Felülírás:** a régi fájl törlése a `S0:<név>` paranccsal, majd új fájl írása. Az `@0:` (save-with-replace) parancs **nem** használható, mert az 1541 DOS-ban hibás.
- **Betöltés utáni törlés:** szintén `S0:<név>`.
- **A játék lemeze:** a szövegeket és a képernyőmodult a játék arról az egységről tölti be, amelyről ő maga betöltődött. Ha valamelyik nem tölthető be, mert nincs a lemezen, csonka, vagy az egység nem válaszol, a 0. sorban „insert the game disk and press space” jelenik meg. Szóközre a játék újra próbálkozik, amíg sikerül.
- **Hiányzó fájlok:** a `rogue.cfg` és a `rogue.scr` hiánya nem hiba: ilyenkor az alapértékek, illetve üres ranglista jön. Mindkettő olyankor olvasódik, amikor a játék lemeze bizonyosan bent van:
  - a `rogue.cfg` induláskor, közvetlenül azután, hogy a játék onnan betöltődött;
  - a ranglista a modulban, közvetlenül a modul betöltése után.

**Mentésfájl formátuma:**
| eltolás | méret | tartalom |
|---|---|---|
| 0 | 8 | `ROGUE64` + 0 |
| 8 | 1 | formátumverzió – minden olyan változtatásnál nő, amely az állapotblokk elrendezését módosítja |
| 9 | 2 | a beállításblokk és az állapotblokk együttes hossza (alacsony bájt előbb) |
| 11 | n | a beállításblokk, utána az állapotblokk, elrejtve (lent) |
| 11+n | 2 | 16 bites összeg az elrejtés előtti bájtokból |

Elrejtés: `c[i] = b[i] XOR key[i mod len(key)] XOR c[i−1]` (`c[−1] = 0`), a kulcs a `"rogue 5.4.4 c64"` szöveg ASCII-bájtjai. Ha az azonosító, a verzió, a hossz vagy az összeg nem egyezik, a betöltés „Sorry, saved game is out of date.” üzenettel leáll ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)).

**Ranglista-fájl:** bejegyzésenként: pontszám (3 bájt), név (16 bájt, nullával lezárva), ok (1 bájt: killed / quit / winner / killed with amulet), szint (1 bájt), halálok (1 bájt: szörnybetű, vagy kód az arrow / bolt / dart / hypothermia / starvation okokra). Az elején 1 bájt: a bejegyzések száma.

**Beállításfájl:** a [4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer) opciói rögzített sorrendben (logikai opciók bitekként, szövegek nullával lezárva), elején egy formátumverzió-bájttal; ismeretlen verziónál a fájl figyelmen kívül marad.

**Sebesség:** a KERNAL soros átvitele kb. 400 bájt/s, így egy kb. 8,5 KB-os mentés kb. 21 másodperc. Írás közben a képernyőn „saving...”, olvasás közben „restoring...”, a szövegek betöltése alatt „loading...” felirat látszik a 0. sorban (ezek csak állapotjelzések, nem üzenetek).

## 10.8 Erőforrás-keretek
| tétel | becslés / cél |
|---|---|
| fő régió (`$0880–$BFFF`) | 46 976 bájt; ebbe kell beférnie a kódnak, az állandó tábláknak és a játékállapotnak. Az RNG szorzótáblái (1 KB) és a pufferek (kb. 0,7 KB) a fő régión kívül vannak ([10.2](#102-memória)). Mérve (2026-11-27): az áthelyezésük a szabad helyet a játékban 4 264-ről 5 984, a fejlesztői buildben 0-ról 1 744 bájtra növelte. A fejlesztői buildben a térképírás ellenőrzésének kivétele és a fejlesztői fájlok méretre fordítása ([10.9](#109-fejlesztői-build-és-determinizmus)) után 2 672 bájt szabad. |
| kód | 2026-11-27-én kb. 31 KB; a hátralévő részre kb. 15–24 KB a becslés (lásd lent, a hely tervénél), mérföldkövenként mérni kell |
| szövegek (üzenetek, nevek, súgó, szótagok) | kb. 11,5 KB tömörítés nélkül, a specifikációból mérve: üzenetek 6,5 KB, nevek és szótagok 2,8 KB, súgó és jelleírások 1,0 KB (a súgósorok jel + leírás formában, a futássorok kiíráskor összerakva), győzelmi képernyő és sírkő 1,1 KB. A fő régión kívül vannak, a `$D000–$FFF9` alatti RAM-ban (12 282 bájt, [10.2](#102-memória)). A címképernyő, a győzelmi képernyő, a sírkő és a ranglista szövegei (kb. 1,3 KB) a képernyőmodulban vannak, így a ROM-ok alatt kb. 10,2 KB foglalt. Mérve (2026-11-24): a szövegek és a tábláik 4 825 bájtot foglalnak a ROM-ok alatt. Az áthelyezésük a betöltővel együtt a szabad helyet a játékban 4 376-ról 8 216, a fejlesztői buildben 192-ről 3 944 bájtra növelte. |
| képernyőmodul | kb. 3 KB kód és szöveg; a játékállapot-blokk helyére töltődik, így nem foglal külön helyet ([10.10](#1010-képernyőmodul)) |
| állandó táblák (szörnyek, tárgyak, erő, xp) | kb. 1,5 KB |
| játékállapot (perzisztens blokk) | kb. 8,5 KB |
| képernyő + karakterkészlet | 3 KB |
| **szintgenerálás** | legfeljebb 3 másodperc |
| **egy kör** (szörnymozgással, rajzolással, görgetés nélkül) | átlagban legfeljebb a futás lépésideje (lásd lent), a szokásos 3–5 ébren lévő szörnnyel; egy-egy kör tovább tarthat (szobába lépés, sok gyorsított vagy repülő szörny). Az alvó szörny nem mozog, nem számít bele. A körök öntesztje (`turns`) minden futáskor méri ([10.9](#109-fejlesztői-build-és-determinizmus)); a szörnymozgás elkészültekor, gyorsítás előtt mélységtől függően átlagosan 2,6–5,3 képkocka volt, nagy részét a szörnyek lépése vitte el (a következő mező és a cél választása), a többit a körülnézés |
| **futás** | lépésenként mindig ugyanannyi képkocka (3 vagy 4, a gyorsítás utáni mérés alapján rögzítve, [10.6](#106-időzítés)) |
| **görgetés** | legfeljebb 2 képkocka |

A szabad memóriát a fordítás írja ki (a `.map` fájlból), a táblák foglaltságát a fejlesztői mód mutatja ([1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)).

**A hely terve** (2026-11-27): a becslés szerint a játék végéig több kód jön, mint amennyi hely van, ezért a lehetőségek fel vannak mérve, és eldőlt, melyik mikor jön.

**A hátralévő kód becslése:** az eredeti forrás függvényei megjegyzések, szóközök és szövegek nélkül, karakterben mérve, készre és hátralévőre osztva. A kész rész a mi kódunkban átlagosan kb. 0,5 bájt eredeti karakterenként, a legutóbb elkészült felszerelés és a szörnyek tárgyai 0,83, mert sok bennük a különleges eset. A hátralévő rész (29 572 karakter) így kb. 15–24 KB kód. A 2026-11-23-i becslés ugyanerre a részre 12,7–18,8 KB volt.

| feladat | KB |
|---|---|
| bájitalok, időzítők, kijelzési hatások | 3,6–5,9 |
| tekercsek | 2,2–3,6 |
| gyűrűk, hajítás | 1,3–2,1 |
| pálcák, sugarak | 2,2–3,7 |
| csapdák, éhség, regeneráció, vándorlók | 1,7–2,8 |
| játék vége, lemez, mentés | 0,9–1,5 |
| beállítások, súgó, jel magyarázata (`/`) | 1,6–2,6 |
| a parancsfeldolgozás hátralévő része | 1,1–1,8 |
| C64-es többletek: kétfeles észlelés-képernyők (E11), lemezírás, a képernyőmodul hívása, a szövegek kibontása, a KERNAL kilapozása | kb. 1,2 |
| táblák, a játékállapot bővülése | 0,5–1 |
| **összesen** | **kb. 16,5–26** |

**A lehetőségek** (a forrás másolatán mérve; a szabad hely a játékban, illetve a fejlesztői buildben; a kör ideje a képkocka százalékában, a szintgenerálás a leglassabb szint képkockában, az 1., 10., 26. és 40. mélységen, [10.9](#109-fejlesztői-build-és-determinizmus)):

| lehetőség | szabad hely | sebesség | tesztek |
|---|---|---|---|
| a mostani állapot | 5 984 / 2 672 bájt | kör: 60 / 53 / 62 / 86%; szint: 58 / 112 / 130 / 110 | átmennek |
| méretre fordítás, szűk kör: `item_names`, `pack`, `text_list`, `discoveries`, `equipment`, `input`, `status`, `player`, `state`, `main`, `platform`, `disk` | +440 / +328 bájt | kör: 1–2%-kal lassabb; szint: változatlan | átmennek |
| méretre fordítás, bővebb kör: a szűk kör, valamint `game`, `combat`, `message`, `monsters`, `items` | +1 432 / +1 376 bájt | kör: 64 / 57 / 66 / 94%, 5–9%-kal lassabb; szint: változatlan | átmennek |
| teljes `-Os` | +3 760 / +3 816 bájt | kör: kb. 25%-kal lassabb; szint: a 26. mélységen 150, a keret határán | egy teszt csak az Oscar64 hibájának megkerülésével megy át (lásd lent) |
| a fejlesztői build kettéosztása (szörnyek és térkép, illetve tárgyak) | – / +848, illetve +1 032 bájt | – | – |
| szövegtömörítés: bájtpár-kódolás, legfeljebb 128 pár a szövegekben nem használt 128–255 kódokon | a ROM-ok alatt: a mostani szövegeken 33%, egy nagyobb katalógusmintán 38–42%; a kibontó kb. 72 bájt; a tervezett kb. 10,2 KB szövegnél kb. 3,3–3,7 KB | a kiolvasás lassulása nincs mérve | – |
| kód a KERNAL alatt (`$E000–$FFF9`, a szövegek után): játék közben kilapozott KERNAL, saját IRQ-belépés | tömörítés nélkül kb. 2 KB, tömörítéssel kb. 5,5 KB | a kód nem lassul; a betöltés KB-onként kb. 2 másodperccel hosszabb | – |
| ritkán használt kód lemezes modulként (mentés és betöltés, beállítások, súgó) | kb. 1–2 KB | betöltés minden használatkor | – |

A méretre fordítás veszélye az Oscar64 egy hibája: ha egy függvény a saját paraméterének címét adja át, és a hívott függvény ezen át megváltoztatja, a hívó a következő hívásnál elavult értéket használ ([README, Oscar64-hibák](../../README.md#oscar64-hibák-és-megkerülésük)). A méretre fordított fájlokban ezért a paramétert előbb helyi változóba kell másolni.

**Döntés**, ebben a sorrendben:
1. Most, a bájitalok előtt: méretre fordítás a bővebb körre. Az új, ritkán futó kód (pl. a bájitalok, a tekercsek, a beállítások) is eleve méretre fordul; a körönként futó és a szintgeneráló kód marad `-O2`.
2. A szövegkatalógus generálása után: szövegtömörítés, a KERNAL játék közben kilapozva, kód a KERNAL alatt.
3. Ha a fejlesztői build újra betelik: a fejlesztői build `-Os`-sel fordul.
4. Végső tartalékként: a játék is `-Os`-sel fordul (a bővebb kör után még kb. +2,3 KB, a kör kb. 25%-kal lassabb), utána a ritkán használt kód lemezes modulként.

A négy lépés a lemezes modulok nélkül kb. 15 KB helyet ad (a mostani 5 984 bájt, a bővebb kör +1,4 KB, a tömörítés és a kód a KERNAL alatt kb. +5,5 KB, a teljes `-Os` kb. +2,3 KB), és a méretre fordított új kód kb. 8%-kal kisebb. Ez a becslés alsó végéhez elég, a felsőhöz nem: kb. 1–10 KB hiányozhat. Ennek egy részét a lemezes modulok adhatják (kb. 1–2 KB), a többire a kód egyszerűsítése jöhet szóba. A becslést mérföldkövenként a mért kódméret alapján újra kell nézni.

## 10.9 Fejlesztői build és determinizmus
- A játéklogika (0–9. fejezet) csak a platformrétegen át éri el a hardvert, így a hardverközeli kód egy helyen van.
- **Fejlesztői build:** külön fordítási beállítással készül. Benne van a játék, a fejlesztői parancsok játék közben, és azok a belső ellenőrzések (hamis feltételnél hibaüzenet a forrásbeli hellyel, majd leállás), amelyekre itt is van hely. A fejlesztői kód ritkán fut, ezért méretre fordul. Induláskor kiírja a seedet, amely itt kézzel is megadható; a RETURN indítja a játékot. A kiadott változatban egyik sincs benne.
- **Tesztprogramok:** az öntesztek és a tesztképernyők nem a fejlesztői buildben vannak, hanem külön programokban, a fejlesztői build beállításával fordítva. A belső ellenőrzések közül azok, amelyekre a fejlesztői buildben nincs hely (most a térképírásé), csak itt vannak. Mindegyikbe csak az kerül, amit a tesztjei hívnak, így a játék és a tesztek nem osztoznak a memórián.
  - A nem interaktív programok a tesztjeiket egymás után lefuttatják, az eredményt a képernyőre írják, és a végén az emulátornak is jelzik (VICE: a hibakereső cartridge kilépési kódja), így egy szkript lefordíthatja, lefuttathatja és kiértékelheti mindet. A hosszú tesztek gyorsított (warp) módban futnak, és ESC-re leállnak; az eredmény ilyenkor az addigi próbákból jön. Ilyen a szint-önteszt: 1000 véletlen szint rögzített magokkal, az 1., 10., 26. és 40. mélységen, majd 100 szinten körök a játékos lépésével és üldöző szörnyekkel, közben egy kör ideje (a képernyőre másolás nélkül); normál sebességen percekig tart.
  - A tesztképernyők egy interaktív programban vannak, menüből: karakterkészlet, billentyűzet, üzenetsáv és nézőablak, státuszsor; mindegyikből az ESC lép vissza a menübe.
- **Fejlesztői parancsok** játék közben az F-billentyűkön (F1–F8, a párosak SHIFT-tel), így egyik játékparancsot sem takarják el. Egyik sem visz el időt.
  - **F1:** új szint: a mélységet a sávban kéri be („depth (1-254): ”); számra arra a mélységre ugrik, üres mezőn RETURN-re új szint jön ugyanazon a mélységen, ESC-re vagy hibás számra elmarad.
  - **F2:** szörny létrehozása betű alapján (A..Z, kis- vagy nagybetűvel; a betűt a sávban kéri be, ESC-re vagy nem betűre elmarad) a játékos melletti szabad mezőre: a mező járható, a játékosról látható, és nem áll rajta szörny. Utána az üzenetsor a szörnytábla foglaltságát mutatja (pl. „monsters: 17/40”); ha nincs szabad mező, vagy betelt a tábla, a szörny nem jön létre, és az üzenet ezzel kezdődik.
  - **F3:** az `inven` opció váltása körbe: clear → overwrite → slow → clear ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)), amíg nincs beállítások menü; az új módot az üzenetsor mutatja (pl. „inventory style: overwrite”).
  - **F4:** a szint összes szörnye felébred és üldöz (`runto`, [6.4](06_szornyek.md#64-ébresztés)); utána az üzenetsor a szörnyek számát mutatja.
  - **F5:** a teljes térkép be- és kikapcsolása. Bekapcsolva a képernyőn a MAP látszik a játékossal és a szörnyekkel: a titkos folyosó-mező inverz `#`, a titkos ajtó inverz `+`, a szörny a betűje, az ébren lévő (ISRUN) inverzen. A térkép minden akció előtt teljesen újrarajzolódik, mert a szörnyek a látott térkép változása nélkül is lépnek és ébrednek. A nézőablak közben is a játékost követi, és új szinten is bekapcsolva marad. A SCREEN réteg közben is frissül, így kikapcsoláskor a normál nézet pontosan visszaáll. Bekapcsolt állapotban más a keret színe.
  - **F6:** a szint összes szörnyének lassítása ki és be: ha van nem lassított szörny, mindegyik lassított (ISSLOW) lesz, különben egyik sem; az új állapotot az üzenetsor mutatja.
  - **F7:** tárgy létrehozása jel alapján (`!` `?` `:` `)` `]` `=` `/` `*` `,`; a jelet a sávban kéri be), altípusos típusnál utána az altípus számát is (a [8.2](08_targyak.md#82-bájitalok-q)–[8.7](08_targyak.md#87-pálcákbotok-z) táblák `#` oszlopa, ételnél 0 = ration, 1 = gyümölcs; a kérdés a tartományt is mutatja, pl. „which (0-13)”); ESC-re, ismeretlen jelre vagy hibás számra elmarad. A tárgy a véletlen generálással azonos kezdőértékeket kap: darabszám, kötegazonosító, bónusz és átok, töltés, gyűrűmód ([8.1](08_targyak.md#81-generálás)–[8.7](08_targyak.md#87-pálcákbotok-z)); az arany mennyisége a mélység szerinti véletlen érték ([2.7](02_szintgeneralas.md#27-arany-és-kezdő-szörnyek-szobánként-nem-fantom-szoba)). A tárgy a játékos melletti szabad mezőre kerül: üres padló- vagy folyosó-mező, a játékosról látható, és nem áll rajta szörny. Utána az üzenetsor a tárgy nevét ([8.8](08_targyak.md#88-azonosítás-és-nevek)), a tárgytábla foglaltságát és a program indulása óta mért csúcsértékét mutatja (pl. „created a blue potion; items: 37/96, peak 52”); ha nincs szabad mező, vagy betelt a tábla, a tárgy nem jön létre, és az üzenet a hibával kezdődik.
  - **F8:** a futás két opciójának váltása körbe ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)), amíg nincs beállítások menü: egyik sem → `follow_passages` → `jump_to_run_end` → mindkettő → egyik sem. Az új állapotot az üzenetsor mutatja (pl. „follow passages: on, jump to run end: off”).
- **A névkészlet foglaltsága:** a `c` parancs után, ha a kérdésére tárgyat kapott ([4.2.6](04_jatekciklus_es_parancsok.md#426-tárgyválasztás-listák-és-tárgyparancsok)), az üzenetsor a névkészlet foglaltságát is mutatja (pl. „names: 5/32”, [1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)).
- **Determinizmus:** ugyanazzal a programváltozattal és ugyanazzal a seeddel ugyanaz a szint és ugyanaz a dobássorozat jön létre, és mentés, majd betöltés után a játék ugyanúgy folytatódik. Az RN-t csak a játéklogika hívhatja; a megjelenítés, a görgetés, az animáció és az időzítés soha.
- A cél az eredeti játékszabályok követése, nem az eredeti program dobássorozatának bájtra pontos utánzása: kódváltozás után ugyanaz a seed más játékot adhat.

## 10.10 Képernyőmodul
A címképernyő, a győzelmi képernyő, a sírkő és a ranglista egy külön lefordított programban, a `rogue.screens` fájlban van, mert ezekre csak a játékon kívül van szükség. A játék a modult a játékállapot-blokk helyére tölti be ([10.2](#102-memória)). Ekkor a blokk tartalmára már nincs szükség: új játéknál újra kitöltődik, betöltésnél a mentésből jön.

- **Mikor töltődik be:**
  - induláskor;
  - minden játék vége után (halál, kilépés, győzelem, sikeres mentés);
  - ha a címképernyő `R` pontja után a betöltés nem sikerül, mert a mentés olvasása a modult felülírhatta.

  A címképernyő `O` pontja után a modul még a memóriában van, nem töltődik újra.
- **Mit csinál:**
  - Játék vége után a végkimenetel szerint megmutatja a sírkövet (`tombstone` opció nélkül a „Killed by…” sort) vagy a győzelmi képernyőt ([4.3](04_jatekciklus_es_parancsok.md#43-szintváltás-és-játék-vége)). Utána jön a „[Press return to continue]”, majd a ranglista beolvasása, frissítése, kiírása és visszaírása. Mentés után ezek elmaradnak.
  - Utána a címképernyő és a menü következik. `S`-re a ranglista jön, `N`-re a név bekérése és a seed előállítása ([10.6](#106-időzítés)).
  - A képernyőket a saját szövegeiből rajzolja ki, nem kész képernyőképként, így kicsi marad és gyorsan töltődik. Később grafika is kerülhet bele, például a címképernyőre.
  - A halálokok nevéhez, a szörnynevekhez is, saját példánya van, mert a ranglista minden bejegyzéséhez kellenek.
- **Visszatérés a játékba:** a modul visszaadja a választást (`N`, `R` vagy `O`), új játéknál a seedet is. A betöltést, a beállítások menüjét és az új játék indítását (a „Hello…” üzenettől) a játék végzi.
- **Átadó blokk:** egy rögzített címen lévő kis terület a fő régióban, a játékállapot-blokkon kívül, rögzített mezőkkel.
  - A játéktól a modulnak: a végkimenetel, a pontszám, a szint és a halálok (szörnybetű vagy kód, mint a ranglistafájlban, [10.7](#107-lemezkezelés)).
  - A modultól a játéknak: a választás és a seed.
- **Beállításblokk:** a beállítások ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)) és a név a játékállapot-blokkon kívül vannak, mert a modul is használja őket: a nevet, a `tombstone` és a `device` opciót. A mentés az állapotblokk előtt ezt is kiírja ([10.7](#107-lemezkezelés)).
- **Közös szerződés:** a két program külön fordul. Egy közös fejlécfájl rögzíti:
  - a modul betöltési és indítási címét;
  - az átadó blokk és a beállításblokk címét és elrendezését.

  Ezekre a modul nem írhat: a játék kódja, az IRQ-kezelő, a képernyő, a karakterkészlet, a szövegek, az RNG szorzótáblái (mert csak induláskor töltődnek fel) és a két blokk. A `$C400–$C7FF` puffereit használhatja, mert a játék ezekbe olvasás előtt mindig ír, az előző üzenetet (CTRL-P) pedig új játéknál és betöltés után üríti ([10.2](#102-memória)). A modulba saját képernyő-, billentyű- és lemezrutinok fordulnak. A játék a modul hívása előtt elmenti a fordító által használt zero page-területet, és visszatérés után visszaírja. Ezeket a hibákat a fordító nem jelzi, ezért a modul első elkészültekor külön ki kell próbálni őket.
- **Betöltés:** a játék lemezéről, arról az egységről, amelyről a játék betöltődött. Ha nem sikerül, „insert the game disk and press space” ([10.7](#107-lemezkezelés)).
- **Későbbi döntés:** az első induláskor a modul a játék PRG-jébe ágyazva eleve a helyén lehetne, így induláskor nem kellene külön betölteni. Ennek feltétele, hogy a játékállapot-blokk ne legyen induláskor nullázott terület. Erről akkor döntünk, amikor a modul elkészült. Címképernyő-grafikával valószínűleg nem fér be.
