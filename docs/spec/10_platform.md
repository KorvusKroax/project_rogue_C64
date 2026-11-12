# 10. Platform: Commodore 64

Ez a fejezet a hardverközeli követelményeket írja le: memória, képernyő, karakterkészlet, billentyűzet, időzítés, lemezkezelés. A játékszabályok (0–9. fejezet) ezektől függetlenek; a program a kettő között egy vékony **platformréteget** használ (képernyő írása, billentyű olvasása, lemezművelet, időzítés), így a hardverközeli kód egy helyen van ([10.9](#109-fejlesztői-build-és-determinizmus)). Ahol egy megoldás csak ajánlás (pl. pontos memóriacímek), ott ez jelölve van; az eszközlánc választása (assembler, C fordító) ezeket módosíthatja.

## 10.1 Célgép
- Commodore 64 vagy 64C, PAL és NTSC; bővítés (REU, gyorstöltő, SuperCPU) nem szükséges, de nem is zavarhat.
- Lemezegység: 1541 / 1571 / 1581 vagy kompatibilis (SD2IEC, emulátor), 8–11. eszközszám.
- A program két PRG fájl: a `rogue` maga a játék, a `rogue.screens` a képernyőmodul ([10.10](#1010-képernyőmodul)). A játék `LOAD"ROGUE",8,1` (vagy `,8` + `RUN`) után indul, egy BASIC-indítósorral (`SYS`), a modult pedig a játék tölti be. A kiadás egy lemezkép, amelyen mindkét fájl rajta van.
- A programból nincs kilépés a BASIC-be: a játék vége után a címképernyő jön ([0.1](00_attekintes.md#01-indítás)), a gépet újraindítással lehet elhagyni. Így a kilépéskor semmit nem kell visszaállítani (megszakításvektorok, ROM-lapozás, a BASIC zero page-e).
- Fejlesztéshez és teszteléshez: VICE (x64sc) emulátor, a kiadás előtt valódi gépen is ki kell próbálni.

## 10.2 Memória
**Konfiguráció:** a BASIC ROM játék közben ki van lapozva, így a `$A000–$BFFF` RAM is használható. A KERNAL ROM és az I/O bent van. Csak egy szöveg kiolvasásának idejére lapozódik ki minden ROM és az I/O is (`$01 = $30`), mert a szövegek a `$D000–$FFFF` alatti RAM-ban vannak (lásd lent).

**Megszakítás:** a saját raszter-IRQ kezelő ([10.6](#106-időzítés)) a `$0314/$0315` vektoron át fut. Szöveg kiolvasása közben a megszakítás tiltva van, így ekkor az IRQ nem fut. A RESTORE billentyű NMI-jét a tiltás nem állítja meg, ezért mindkét helyen egy azonnal visszatérő kezelőre mutat: a KERNAL-lal a `$0318/$0319` vektoron át, kilapozott ROM mellett a RAM-beli `$FFFA/$FFFB` vektoron át. Emiatt a `$FFFA–$FFFF` 6 bájtjára nem kerülhet szöveg.

**Ajánlott memóriatérkép:**

| tartomány | tartalom |
|---|---|
| `$0000–$00FF` | zero page – a KERNAL lemezrutinjai által használt címeket meg kell hagyni, vagy lemezművelet előtt elmenteni |
| `$0100–$01FF` | hardveres verem (256 bájt: rekurzió nincs, [2.5](02_szintgeneralas.md#25-szobák-rajzolása)) |
| `$0200–$03FF` | KERNAL munkaterület |
| `$0400–$07FF` | szabad (az alapértelmezett képernyő áthelyezve): pl. pufferek, bevitel |
| `$0801–$087F` | BASIC-indítósor |
| `$0880–$BFFF` | fő régió (46 976 bájt): kód, állandó adatok, változók, verem; a játékállapot-blokk helyére a címképernyő idején a képernyőmodul töltődik ([10.10](#1010-képernyőmodul)) |
| `$C000–$C3FF` | képernyőmemória (VIC 3. bank) |
| `$C400–$C7FF` | szabad (1 KB, lapozás nélkül elérhető): pl. pufferek, ritkán használt táblák |
| `$C800–$CFFF` | saját karakterkészlet (2 KB) |
| `$D000–$DFFF` | I/O, színmemória (`$D800`); alatta RAM: szövegek |
| `$E000–$FFF9` | KERNAL ROM; alatta RAM: szövegek |
| `$FFFA–$FFFF` | KERNAL ROM; alatta RAM: az NMI-, RESET- és IRQ-vektor a kilapozott ROM idejére |

**Szövegek:** minden játékszöveg ([1.4.4](01_adatmodell_es_flagek.md#144-szövegek)) a `$D000–$FFF9` alatti RAM-ban van (12 282 bájt; a rezidens szövegek kb. 10,2 KB-ot foglalnak). Ha egyszer nem férnének el, a `$C400–$C7FF` jöhet szóba a folytatásukra.
- **Kiolvasás:** a megszakítás tiltása, `$01 = $30`, a szöveg átmásolása egy pufferbe a fő régióban, a `$01` visszaállítása, végül a megszakítás engedélyezése.
- **Betöltés:** a `$D000` alá a betöltés nem írhat, mert ott az I/O látszik. Ezért a PRG-ben a szövegek a kód után vannak, és induláskor `$01 = $30` mellett a helyükre másolódnak. Az ideiglenes példány helyét ezután a változók használják.
- **Tömörítés:** nincs. Ha a szövegek kinőnék a helyet, akkor jöhet szóba, például szótáras tömörítés.

**Játékállapot:** a teljes perzisztens állapot ([1.4.5](01_adatmodell_es_flagek.md#145-térképrétegek)) egyetlen összefüggő blokk (kb. 8,5 KB), mert a mentés és a betöltés ezt egyben írja és olvassa ([10.7](#107-lemezkezelés)). Ami nem kerül bele:
- a beállításblokk, mert a képernyőmodul is használja; a mentés ettől még kiírja ([10.10](#1010-képernyőmodul));
- a videomemória, a pufferek, az állandó táblák és a szövegek.

## 10.3 Képernyő és karakterkészlet
- **VIC beállítás:** 3. bank (`$C000–$FFFF`), képernyő `$C000`, karakterkészlet `$C800` (ajánlás). Szöveges mód, 40×25, normál (nem többszínű, nem kiterjesztett háttérszínű) karakterek.
- **Színek:** keret és háttér fekete, minden karakter világosszürke (15); a színmemória induláskor egyszer kitöltődik. (Színes jelek: opcionális kiegészítés, a játékszabályokat nem érinti.)
- **Karakterkészlet** (2 KB, 256 jel): a 32..126. jel az azonos ASCII-kódú karakter (ASCII-hű betűkészlet, kis- és nagybetűkkel, a PETSCII-ből hiányzó `\` `^` `_` `{` `|` `}` `~` jelekkel is); a 128..254. jel az előző fél inverze (bitenkénti negálás). A 0..31. jel szabad (pl. keretrajzolás az *overwrite* listához, vagy üresen). A 127. és 255. jel nem használt.
- **Írás:** a videomemóriába közvetlenül a jel kódja kerül (ASCII, inverznél +128); a KERNAL képernyőrutinjai (CHROUT a képernyőre) nem használhatók.
- **Sor törlése a végéig**, **inverz megjelenítés**, **teljes képernyő törlése**: a platformréteg egyszerű műveletei.
- A görgetéshez ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)) a térképterület (22 × 40 = 880 bájt) újrarajzolása a SCREEN rétegből egy képkockán belül elvégezhető; hardveres finom görgetés nem kell.

## 10.4 Billentyűzet
- **Mátrixolvasás:** a raszter-IRQ minden képkockában végigolvassa a 8×8-as billentyűmátrixot (CIA1 `$DC00` oszlopkiválasztás, `$DC01` sorolvasás). A KERNAL billentyűrutinja (SCNKEY/GETIN) nem használatos, ezért az IRQ nem a `$EA31`-re ugrik: a jiffy órát az UDTIM (`$FFEA`) lépteti, a visszatérés a `$EA81`-en át történik.
- **Módosítók:** bal SHIFT, jobb SHIFT, SHIFT LOCK (a bal SHIFT-tel azonos mátrixpont), CTRL, C=. A logikai billentyű a lenyomott nem-módosító billentyűből és a módosítókból áll elő a [4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet) táblázata szerint: egy-egy táblázat adja a billentyű ASCII-kódját módosító nélkül és SHIFT-tel; CTRL vagy C= + betű = a betű kódja `& $1F`.
- **Leütés:** egy billentyű akkor számít leütésnek, ha az előző olvasáskor nem volt lenyomva; a még nyomva tartott billentyűk nem takarják el. Egy képkockában legfeljebb egy leütés számít: az új billentyűk közül a mátrix-sorrendben első, a többi a következő képkockákban, így egyik sem vész el. A módosítók és a kód nélküli billentyűk (kurzor, F, CLR/HOME) nem számítanak. A ghosting (szellemleütés) kezelése nem szükséges.
- **Ismétlés:** a parancsra váró állapotban 16 képkocka nyomva tartás után ismétlődik, utána 4 képkockánként ([4.2.0](04_jatekciklus_es_parancsok.md#420-billentyűzet)). A játék csak a parancsra várakozás idejére kapcsolja be, és az ismételt kód csak üres pufferbe kerül.
- **Puffer:** 8 elemű körpuffer, mind a 8 hely használható; az IRQ ír bele, a játék olvas belőle. Tele pufferbe érkező leütés eldobódik. Ürítés: „billentyűpuffer ürítése” és a `flush` opció.
- **RUN/STOP:** ESC-ként viselkedik, nem szakítja meg a programot.
- **Botkormány:** az 1. porti botkormány a billentyűmátrix vonalait zavarja; ha opcionálisan támogatott, a 2. portot kell használni.

## 10.5 Aritmetika
- **RN:** `seed = seed × 11109 + 13849` (mod 2³²): 32×16 bites szorzás + 32 bites összeadás; az eredmény felső 16 bitje az RN ([0.2](00_attekintes.md#02-alapprimitívek)).
  - **Bájtonkénti szorzótáblák:** a seed minden bájtjának a szorzó alsó (`$65`) és felső (`$2B`) bájtjával vett szorzata táblából jön (4 × 256 bájt, induláskor feltöltve, `rng_init`), és a szorzat bájtoszloponként, átvitellel adódik össze, mint az írásbeli szorzásnál; a 3. bájt fölötti rész elveszik. Egy `RN` kb. 170 µs (az Oscar64 általános 32 bites szorzójával kb. 750 µs), bitre ugyanazzal a sorozattal.
  - **Ha kell a memória:** tábla nélkül eltolással és összeadással is számolható (`$2B65`: az alsó bájt 0., 2., 5., 6., a felső 0., 1., 3., 5. bitje), kb. 410 µs.
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
- **Futás üteme:** futás közben (`jump` nélkül) minden lépés után a játék a következő ütem elejéig vár, így minden lépés ugyanannyi képkockáig tart (1 vagy 2, [10.8](#108-erőforrás-keretek)), akár van szörny a közelben, akár nincs. `jump` mellett nincs várakozás, mert a futás közben nincs rajzolás.
- **Animációs késleltetés:** a dobott tárgy és a lövedék lépésenként 2 képkockát vár, a sugár (7.6) lépésenként 2-t, és a teljes sugár a visszarajzolás előtt még 10 képkockáig látszik. A nézőablakon kívüli lépések nem várnak ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).
- **Seed:** a címképernyőn az `N` lenyomásakor a 32 bites seed bájtjai: CIA1 A időzítő alsó bájtja, felső bájtja, a raszter-sor (`$D012`) és a jiffy-számláló alsó bájtja, mindegyik XOR-olva a jiffy-számláló többi bájtjával ([0.2](00_attekintes.md#02-alapprimitívek)). Ha az eredmény 0, a seed 1.
- A játéklogika soha nem függ az időtől: az időzítés csak a megjelenítést és a seed kezdőértékét érinti.

## 10.7 Lemezkezelés
**Rutinok:** a KERNAL szabványos rutinjai (SETLFS, SETNAM, OPEN, CHKIN/CHKOUT, CHRIN/CHROUT, READST, CLRCHN, CLOSE), a `device` opció egységszámával ([4.2.5](04_jatekciklus_es_parancsok.md#425-rendszer)). Lemezművelet közben a KERNAL be van lapozva.

**Fájlok** (a képernyőmodul PRG, a többi SEQ):
| fájl | tartalom |
|---|---|
| `rogue.screens` | képernyőmodul ([10.10](#1010-képernyőmodul)) |
| a `file` opció neve (alapból `rogue.sav`) | mentett játék |
| `rogue.scr` | ranglista (legfeljebb 10 bejegyzés) |
| `rogue.cfg` | beállítások és a legutóbbi név |

- **Fájlnevek:** ASCII-ból PETSCII-re alakítva: `a`–`z` → `$41–$5A`, `A`–`Z` → `$C1–$DA`, a többi nyomtatható karakter változatlan. A fájlnév nem tartalmazhat `:` `,` `*` `?` `=` `@` karaktert (ezekre a bekérés nem enged).
- **Hibacsatorna:** minden művelet után a 15. csatornáról a `kód,szöveg,sáv,szektor` sor olvasható; 00–19 rendben, egyébként „disk error: <kód>,<szöveg>”. Ha az egység nincs bekapcsolva (READST: `DEVICE NOT PRESENT`), az üzenet „disk error: device not present”.
- **Létezés-ellenőrzés:** a fájl megnyitása olvasásra, majd a hibacsatorna: `00` → létezik, `62` → nem létezik.
- **Felülírás:** a régi fájl törlése a `S0:<név>` paranccsal, majd új fájl írása. Az `@0:` (save-with-replace) parancs **nem** használható, mert az 1541 DOS-ban hibás.
- **Betöltés utáni törlés:** szintén `S0:<név>`.
- **A játék lemeze:** ha a képernyőmodul nem tölthető be, mert nincs a lemezen, vagy az egység nem válaszol, a 0. sorban „insert the game disk and press space” jelenik meg. Szóközre a játék újra próbálkozik, amíg sikerül.
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

**Sebesség:** a KERNAL soros átvitele kb. 400 bájt/s, így egy kb. 8,5 KB-os mentés kb. 21 másodperc. Írás közben a képernyőn „saving...”, olvasás közben „restoring...” felirat látszik a 0. sorban (ezek csak állapotjelzések, nem üzenetek).

## 10.8 Erőforrás-keretek
| tétel | becslés / cél |
|---|---|
| fő régió (`$0880–$BFFF`) | 46 976 bájt; ebbe kell beférnie a kódnak, az állandó tábláknak és a játékállapotnak |
| kód | 28–34 KB; durva becslés, nem mérés, mérföldkövenként mérni kell |
| szövegek (üzenetek, nevek, súgó, szótagok) | kb. 11,5 KB tömörítés nélkül, a specifikációból mérve: üzenetek 6,5 KB, nevek és szótagok 2,8 KB, súgó és jelleírások 1,0 KB (a súgósorok jel + leírás formában, a futássorok kiíráskor összerakva), győzelmi képernyő és sírkő 1,1 KB. A fő régión kívül vannak, a `$D000–$FFF9` alatti RAM-ban (12 282 bájt, [10.2](#102-memória)). A címképernyő, a győzelmi képernyő, a sírkő és a ranglista szövegei (kb. 1,3 KB) a képernyőmodulban vannak, így a ROM-ok alatt kb. 10,2 KB foglalt. |
| képernyőmodul | kb. 3 KB kód és szöveg; a játékállapot-blokk helyére töltődik, így nem foglal külön helyet ([10.10](#1010-képernyőmodul)) |
| állandó táblák (szörnyek, tárgyak, erő, xp) | kb. 1,5 KB |
| játékállapot (perzisztens blokk) | kb. 8,5 KB |
| képernyő + karakterkészlet | 3 KB |
| **szintgenerálás** | legfeljebb 2 másodperc |
| **egy kör** (szörnymozgással, rajzolással, görgetés nélkül) | legfeljebb a futás lépésideje (lásd lent), a szokásos 3–5 ébren lévő szörnnyel; a szörnymozgás elkészültekor mérni kell. Az alvó szörny nem mozog, nem számít bele |
| **futás** | lépésenként mindig ugyanannyi képkocka (1 vagy 2, a mérés alapján rögzítve, [10.6](#106-időzítés)) |
| **görgetés** | legfeljebb 2 képkocka |

A szabad memóriát a fordítás írja ki (a `.map` fájlból), a táblák foglaltságát a fejlesztői mód mutatja ([1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)).

## 10.9 Fejlesztői build és determinizmus
- A játéklogika (0–9. fejezet) csak a platformrétegen át éri el a hardvert, így a hardverközeli kód egy helyen van.
- **Fejlesztői build:** külön fordítási beállítással készül. Benne vannak a fejlesztői mód eszközei (a menüben és a tesztképernyőkön, valamint fejlesztői parancsként játék közben), a belső ellenőrzések (hamis feltételnél hibaüzenet a forrásbeli hellyel, majd leállás) és az öntesztek (pl. sok szint generálása és ellenőrzése). A kiadott változatban egyik sincs benne. A hosszú öntesztek VICE-ben, gyorsított (warp) módban futnak. A gyors öntesztek minden indításkor lefutnak, alattuk egy menü: innen indulnak billentyűre a szint-önteszt (1000 véletlen szint rögzített magokkal, az 1., 10., 26. és 40. mélységen; normál sebességen percekig tart) és a tesztképernyők, bármelyik bármikor, és itt adható meg kézzel a seed; mindegyikből az ESC lép vissza a menübe, a RETURN pedig a játékot indítja.
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

  Ezekre a modul nem írhat: a játék kódja, az IRQ-kezelő, a képernyő, a karakterkészlet, a szövegek és a két blokk. A modulba saját képernyő-, billentyű- és lemezrutinok fordulnak. A játék a modul hívása előtt elmenti a fordító által használt zero page-területet, és visszatérés után visszaírja. Ezeket a hibákat a fordító nem jelzi, ezért a modul első elkészültekor külön ki kell próbálni őket.
- **Betöltés:** a játék lemezéről, arról az egységről, amelyről a játék betöltődött. Ha nem sikerül, „insert the game disk and press space” ([10.7](#107-lemezkezelés)).
- **Későbbi döntés:** az első induláskor a modul a játék PRG-jébe ágyazva eleve a helyén lehetne, így induláskor nem kellene külön betölteni. Ennek két feltétele van:
  - a játékállapot-blokk nem lehet induláskor nullázott terület;
  - a modulnak a szövegek ideiglenes példánya mellett is be kell férnie a fő régióba.

  Erről akkor döntünk, amikor a modul és a szövegek kimásolása elkészült. Címképernyő-grafikával valószínűleg nem fér be.
