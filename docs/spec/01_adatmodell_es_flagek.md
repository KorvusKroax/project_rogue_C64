# 1. Adatmodell és flagek

Az 1.1–1.3 a játék logikai adatmodellje; az [1.4](#14-ábrázolás-c64-en) írja le, hogyan kerül mindez a C64 memóriájába (táblák, mezőszélességek, korlátok).

## 1.1 Rekordok

### 1.1.1 Room (9 db, szektoronként egy)
```
Room {
  pos: (x,y)          // bal felső sarok (fallal együtt); fantom szobánál maga a folyosó-pont
  size: (w,h)         // méret falakkal együtt
  flags:
    PHANTOM           // "gone" szoba (ISGONE): csak egy folyosó-csomópont
    DARK              // a szoba sötét (ISDARK)
    MAZE              // labirintus (ISMAZE)
  // a kijáratok a közös ajtótáblában vannak (1.4.7)
  gold_pos, gold_val  // arany a szobában (ha van)
}
// összekötéshez (csak generálás közben): conn[9][9] szomszédsági tábla, isconn[9][9], ingraph[9]
// folyosórendszerek: passages[] – minden összefüggő folyosóhálózat külön "szobaként" szerepel
//   (flags = PHANTOM|DARK), ez kell a drain life-hoz és a szörnyek útkereséséhez; legfeljebb MAX_PASSAGES
//   – csak az A változatban, a B változatban nincs ilyen (1.4.7)
```

### 1.1.2 Creature (játékos és szörny közös rekord)
```
Creature {
  str       // erő (3..31); szörnynél mindig 10 (6.1) – szörnynél nem kell tárolni
  exp       // játékos: összegyűjtött tapasztalat; szörny: ennyi XP-t ad, ha megölik (6.2)
  lvl       // szint; játékos: a tapasztalatból (5.2), szörny: típus + mélységi módosító (6.2)
  arm       // páncélosztály: minél kisebb, annál nehezebb eltalálni (játékos alapból 10)
  dmg       // sebzés-string ("NxM/NxM", 0.2); játékosnál fegyver nélküli ütés ("1x4"); szörnynél a típusé (6.1)
  hpt       // aktuális életerő
  maxhp     // maximális életerő
  pos       // pozíció a térképen
  room      // a szoba vagy folyosórendszer, amelyben áll (1.1.1); a B változatban a szoba vagy „folyosón” (1.4.7)
  flags     // állapotflagek (1.2.1 játékos, 1.2.2 szörny)
  pack      // list<Item>: játékosnál a csomag, szörnynél a nála lévő tárgyak
  oldch     // a mező képernyőjele a lény alatt; ezt kell visszarajzolni, ha elmegy vagy nem látszik
  // csak szörny:
  type      // 'A'..'Z': a típus betűje, index a típustáblába (6.1)
  dest      // hová tart: a játékos vagy egy padlón fekvő tárgy (6.5)
  disguise  // amilyen jellel kirajzolódik: a saját betűje, a Xeroc álcázva tárgyjel (6.2, 6.7)
  turn      // bool: ISSLOW szörnynél váltogat, csak true-nál lép (6.2)
}
// a szörny neve és carry értéke a típustáblából jön (6.1), nem kell a rekordban tárolni
// a Venus flytrap számlálója (vf_hit) nem a lényhez tartozik: globális, a GameState-ben van
```
Játékos-specifikus állapot a `GameState`-ben (lásd lent).

### 1.1.3 Item
```
Item {
  type     // kategória: POTION, SCROLL, FOOD, WEAPON, ARMOR, STICK, RING, GOLD, AMULET
  which    // altípus index a kategórián belül (a 8.2–8.7 táblák # oszlopa); ételnél 0 = ration, 1 = gyümölcs
  pos      // pozíció, ha a padlón van
  packch   // csomagbetű ('a'..'z'), ha a csomagban van (8.9)
  launch   // a kilövő fegyver típusa, amellyel lőni kell (nyílnál: short bow); nincs: üres (7.4) – nem tárolt, a típusból jön
  damage   // sebzés-string közelharcban, kézben tartva ("NxM") – nem tárolt: fegyvernél a típusból (8.4), pálcánál a stick_is_staff-ból (8.7)
  hurldmg  // sebzés-string dobva vagy kilőve (7.4) – nem tárolt, mint a damage
  count    // darabszám kötegelt tárgynál (nyíl, dart, shuriken, dagger, bájital, tekercs, étel)
  hplus    // találati bónusz (fegyvernél; átkozott fegyvernél negatív) (7.1, 8.4)
  dplus    // sebzésbónusz (fegyvernél) (7.1, 8.4)
  arm      // jelentése típusonként: páncélnál védelem (kisebb = jobb); pálcánál töltés; aranynál az összeg; gyűrűnél a mod (pl. protection +1)
  flags    // ISCURSED, ISKNOW, ISMISL, ISMANY, ISFOUND, ISPROT (1.2.3); az ISMISL és az ISMANY nem tárolt, a típusból jön
  group    // kötegazonosító: azonos which + group → egy csomaghelyen összevonódik (8.4, 8.9)
  label    // a játékos által adott egyedi név ('c'), fegyvernél, páncélnál, amulettnél (4.2.6); index a névkészletbe (1.4.1)
}
// az azonosítatlan név (bájitalszín, tekercsfelirat, gyűrűkő, pálcaanyag) a típushoz tartozik
// (GameState.names), nem a tárgyhoz – nem kell tárgyanként tárolni
```
A tárolt rekordot az [1.4.6](#146-tárgyrekord) írja le.

### 1.1.4 GameState
```
GameState {
  level                  // a jelenlegi dungeon-szint (mélység), 1..255 (E16)
  max_level              // a legmélyebb eddig elért szint; tárgy/arany csak ott generálódik, ha nálad az amulett (2.7, 2.8)
  gold                   // a játékos aranya; ebből lesz a pontszám (4.3)
  player                 // Creature: a játékos (1.1.2)
  cur_weapon             // a kézben tartott fegyver (vagy üres)
  cur_armor              // a viselt páncél (vagy üres)
  cur_ring[LEFT]         // gyűrű a bal kézen (vagy üres) (8.6)
  cur_ring[RIGHT]        // gyűrű a jobb kézen (vagy üres)
  food_left              // élelemkészlet: körönként fogy, evéskor nő; 300 alatt Hungry, 150 alatt Weak, 0 alatt ájulás (5.4)
  hungry_state           // 0 = rendben, 1 = Hungry, 2 = Weak, 3 = Faint (5.4)
  no_food                // hány szint óta nem generálódott étel; > 3 esetén a következő tárgy étel (8.1)
  regen_counter          // HP-regeneráció számlálója (5.3)
  vf_hit                 // Venus flytrap fogásszámláló: ennyit sebez a fogva tartás alatt (6.7)
  has_amulet             // a játékosnál van-e az amulett (8.10)
  str_max                // a játékos legnagyobb elért ereje gyűrűk nélkül; a státuszsoron "Str:cur(max)" (5.6)
  timers: {              // hátralévő körök az egyes hatásokból (4.1.1)
    confused             // zavarodottság (ISHUH)
    hallucinating        // hallucináció (ISHALU)
    see_invis            // láthatatlan-látás a bájitaltól (CANSEE, J5)
    blind                // vakság (ISBLIND)
    levitating           // lebegés (ISLEVIT)
    hasted               // gyorsítás (ISHASTE)
    no_command           // alvás/ájulás/fagyás: ennyi körig nem cselekedhet a játékos
    bear_trap            // ennyi körig nem léphet el a medvecsapdából (9.1)
    monster_detect       // szörnyérzékelés (SEEMONST)
  }
  wander_timer           // időzítő a vándorló szörny démon indulásáig (6.2)
  wander_check_tick      // a vándorló démon körszámlálója (between, 6.2)
  know[type][which]      // a tárgytípus azonosítva van-e (8.2, 8.3)
  guess[type][which]     // a játékos által a típusnak adott név ('c'), ha még nincs azonosítva; index a névkészletbe (1.4.1)
  names[type][which]     // azonosítatlan nevek ebben a játékban: színek, kövek, anyagok (8.8)
  scroll_title_seed      // ebből állnak elő a tekercsfeliratok minden kiíráskor; a feliratok nem tárolódnak (8.8)
  stick_is_staff[which]  // az adott pálcatípus ebben a játékban bot (staff) vagy pálca (wand) (8.8)
  rooms[9], passages[]   // a 9 szoba és a folyosórendszerek (1.1.1; passages[] csak az A változatban, 1.4.7)
  exits                  // a közös ajtótábla (1.4.7)
  monsters               // a szinten lévő szörnyek listája
  floor_items            // a padlón lévő tárgyak listája
  last_command           // az előző parancs, az 'a' ismétléshez (4.2.5)
  last_message           // az utolsó üzenet, a CTRL-P-hez (3.4)
  count_prefix           // a parancs előtti számprefix (pl. "5s") (4.2.5)
  can_huh                // CANHUH: a következő sikeres ütés összezavarja a szörnyet (8.3)
  max_hit_this_fight     // az aktuális harc legnagyobb egyszeri sebzése, az 'f' leállásához (7.2)
  view_window_x                 // a nézőablak bal szélének logikai oszlopa, 0..40 (3.5)
  options                // a beállítások (4.2.5); a GameState-en kívül, külön blokkban, mert a képernyőmodul is használja, de a mentésbe bekerül (10.10)
  seed                   // az RN állapota (0.2)
}
```

> **C64-eltérés (E14):** az eredeti `turn` (körszámláló), a szint csapdalistája (`traps`), az `F_DROPPED` mezőflag és az `ISREGEN` szörnyflag a játékban semmire nem hat, ezért elhagyható. A csapda típusa a mező flagjében van (1.2.4), a csapdák száma csak a generáláskor kell.

---

## 1.2 Flagek

### 1.2.1 Játékos
| flag | jelentés | be | ki |
|---|---|---|---|
| ISRUN | „ébren van” – egyetlen hatása: ha hiányzik, a szörnyek +4-gyel találják el (a cselekvőképességet a `no_command` számláló kezeli) | alap; ájulás/alvás/fagyás lejártakor | alvás (tekercs, gáz), fagyás, gyorsítás-ájulás, éhség-ájulás |
| ISHUH | zavarodott: mozgásnál 80% véletlen irány (`rndmove`, [6.6](06_szornyek.md#66-mozgás): előbb `dy`, aztán `dx = rnd(3)-1`; (0,0) nem visz időt), dobásnál/`^`-nál 20% véletlen irány | bájital, Medusa | időzítő; időtartamok összeadódnak |
| CANHUH | a következő sikeres ütés összezavarja a szörnyet | Monster confusion tekercs | első sikeres ütés (közel vagy dobás) |
| CANSEE | látja a láthatatlant; **C64-javítás (J5):** akkor áll fenn, ha a see invisible időzítő fut *vagy* see invisible gyűrű van valamelyik kézen | bájital, gyűrű | ha egyik forrás sincs már |
| ISBLIND | vak: térkép nem frissül | bájital | időzítő, gyógyítás |
| ISLEVIT | lebeg: csapdák hatástalanok, nem ébreszt, nem lépcsőzhet, nem vehet fel | bájital | időzítő |
| ISHALU | hallucinál: szörnyek/tárgyak véletlen jellel, minden körben újrarajzolva | bájital | időzítő, extra gyógyítás |
| SEEMONST | szörnyek látszanak falon át (inverz), ISINVIS nem | bájital (20 kör) | időzítő |
| ISHASTE | 2 akció / kör | bájital | időzítő |
| ISHELD | nem léphet el a mezőről (harc, tárgyhasználat mehet) | Venus flytrap | flytrap halála, teleport, egyes pálcák flytrapra |

*C64-javítás (J1):* az eredetiben az éhség-állapotváltás is törölte az ISRUN-t ([5.4](05_jatekos.md#54-éhség)).

### 1.2.2 Szörny
| flag | jelentés |
|---|---|
| ISMEAN | belépéskor felébredhet (lásd [6.4](06_szornyek.md#64-ébresztés)) |
| ISFLY | ha lépés után dist2(játékos) ≥ 3, még egyszer lép |
| ISGREED | előbb a szobabeli aranyra megy és ráül |
| ISINVIS | csak CANSEE mellett látszik |
| ISSLOW | csak minden második körben cselekszik (`turn` váltogat) |
| ISCANC | speciális képesség kikapcsolva |
| ISFOUND | speciális képességet már elsütötte |
| ISTARGET | `f` parancs célpontja |
| ISRUN | üldöz (nélküle a szörny egyáltalán nem mozog) |
| ISHUH | 80% véletlen mozgás; minden zavart lépésnél 1/20 eséllyel feloldódik |
| ISHASTE | 2 akció / kör |
| ISHELD | egyáltalán nem mozog (Hold monster tekercs) |

Az eredeti ISREGEN flag (griff, troll, vámpír) hatástalan, elhagyható (E14).

### 1.2.3 Tárgy
| flag | jelentés |
|---|---|
| ISCURSED | használatban lévő tárgy nem vehető le / nem dobható |
| ISKNOW | azonosítva: bónuszok, töltésszám látszik |
| ISMISL | lövedék (nyíl íjjal, magic missile) |
| ISMANY | kötegelt: nyíl, dobótőr, shuriken, arany |
| ISFOUND | a tárgy már volt a csomagban (minden felvételkor beáll). A scare monster tekercs ebből tudja, hogy a következő felvételkor porrá kell válnia ([8.3](08_targyak.md#83-tekercsek-r)) |
| ISPROT | páncél véd a rozsdától |

### 1.2.4 Mező
| flag | jelentés |
|---|---|
| F_PASS | folyosó vagy labirintus mező |
| F_SEEN | a játékos már látta a mezőt |
| F_REAL | nem rejtett („amit látsz, az van ott”); ha hiányzik, a mező rejtett (lásd lent) |
| F_PNUM | a folyosórendszer sorszáma (alsó 4 bit), folyosó-mezőn; a B változatban nincs használatban, csak a drain life kitöltése jelöl vele ideiglenesen ([1.4.7](#147-kijáratok-és-folyosók)) |
| F_TMASK | a csapda típusa (alsó 3 bit), csapda-mezőn ([9.1](09_csapdak.md#91-rálépés)) |
| F_MONST | a mezőn szörny áll; hogy melyik, azt a szörnylista adja meg ([1.4.5](#145-térképrétegek)) |

*Megjegyzés:* az F_PNUM és az F_TMASK ugyanazokat a biteket használja, ezért egy mező nem lehet egyszerre folyosó és csapda. Emiatt nem kerül csapda labirintusba ([2.9](02_szintgeneralas.md#29-csapdák)). Egy mezőflag-bájt elég: F_PASS, F_SEEN, F_REAL, F_MONST és 4 bit F_PNUM/F_TMASK, ezzel mind a 8 bit foglalt.

**Rejtett mező** (F_REAL nélkül) megjelenése, amíg fel nem fedezik:

| mi van ott valójában | minek látszik |
|---|---|
| titkos ajtó | az adott fal karaktere (`-` vagy `\|`) |
| titkos folyosó | SOLID_ROCK (üres) |
| rejtett csapda | FLOOR |

---

## 1.3 Térkép és képernyő

### 1.3.1 Logikai térkép
| név | érték |
|---|---|
| MAP_COLUMNS × MAP_ROWS | 80 × 22 – **logikai** méret |
| Térkép-mezők | x = 0..79, y = 1..22 (a 0. sor az üzenetsáv, a 23. a státuszsor) |

A térkép két rétegű: a **MAP** a valódi szint (karakter és flagek mezőnként; hogy melyik szörny áll egy mezőn, azt az F_MONST flag és a szörnylista adja meg), a **SCREEN** az, amit a játékos lát és amire emlékszik. Megjelenítéskor a MAP-ből másolunk a SCREEN-re. A SCREEN teljes, 80×22-es réteg a memóriában; a képernyőre ebből csak a nézőablak kerül ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).

### 1.3.2 Fizikai képernyő

> **C64-eltérés (E1, E3):** az eredeti 80×24-es terminál helyett a C64 40×25-ös szöveges képernyője.

| képernyősor | tartalom |
|---|---|
| 0. | üzenetsor ([3.4](03_latoter_es_megjelenites.md#34-üzenetsor)) |
| 1..22. | a térkép nézőablaka: a logikai `y` sor a képernyő `y`. sorában, a logikai `x` oszlop a képernyő `x − view_window_x` oszlopában ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)); az 1. sort egy hosszú üzenet ideiglenesen eltakarhatja ([3.4](03_latoter_es_megjelenites.md#34-üzenetsor)) |
| 23..24. | státuszsor, két sorban ([3.3](03_latoter_es_megjelenites.md#33-státuszsor)) |

A teljes képernyős megjelenítések (csomaglista *clear* módban, súgó, sírkő, ranglista, győzelem) a 0..24. sort használják.

### 1.3.3 Karakterek
| konstans | jel | | konstans | jel |
|---|---|---|---|---|
| PASSAGE | `#` | | FOOD | `:` |
| DOOR | `+` | | WEAPON | `)` |
| FLOOR | `.` | | ARMOR | `]` |
| PLAYER | `@` | | AMULET | `,` |
| TRAP | `^` | | RING | `=` |
| STAIRS | `%` | | STICK | `/` |
| GOLD | `*` | | MAGIC | `$` (csak mágia-észlelés) |
| POTION | `!` | | SOLID_ROCK | ` ` |
| SCROLL | `?` | | MONSTER | `A`..`Z` |
| WALL_HORIZONTAL | `-` | | WALL_VERTICAL | `\|` |

Nincs külön sarok-karakter: a felső és alsó fal a sarkokkal együtt `-`, az oldalfalak `|`.

A jelek **ASCII-kódok**: a saját karakterkészletben a karakter indexe az ASCII-kód (32..126), és a 128-cal eltolt index ugyanannak az inverz változata ([10.3](10_platform.md#103-képernyő-és-karakterkészlet)). Így a MAP, a SCREEN és a videomemória ugyanazt a kódot tárolja, a szövegek is ASCII-ban vannak (PETSCII-átalakítás csak a lemezes fájlneveknél kell, [10.7](10_platform.md#107-lemezkezelés)). A SCREEN rétegben a 7. bit az inverz megjelenítést jelzi (SEEMONST).

---

## 1.4 Ábrázolás C64-en

> **C64-eltérés (E10, E12):** az eredeti dinamikus listái és tetszőleges hosszú szövegei helyett fix méretű táblák és korlátos hosszak.

### 1.4.1 Táblák és listák
Minden rekordfajta egy fix méretű táblában (pool) van; a rekordokra 1 bájtos **index** hivatkozik (0 = nincs). A listák (szörnylista, padlón lévő tárgyak, csomag, egy szörny csomagja) egyszeresen láncoltak a rekordok `next` indexmezőjén át, így a sorrendjük pontosan az eredetié (pl. az új szörny a lista **elejére** kerül, [6.2](06_szornyek.md#62-generálás)) – ez a dobássorrend és a determinizmus miatt kötelező.

| tábla | méret | megjegyzés |
|---|---|---|
| szörnyek | MAX_MONSTERS = 40 | 9 kezdő + kincses szoba (≤ 12) + vándorlók és teremtett szörnyek |
| tárgyak | MAX_ITEMS = 96 | padló, csomag és szörnycsomagok együtt; az arany is tárgy; rekordonként 11 bájt ([1.4.6](#146-tárgyrekord)); a fejlesztői mód foglaltsága alapján később csökkenthető |
| nevek (`guess`, `label`) | MAX_NAMES = 32 | közös névkészlet, egy-egy ≤ 20 karakteres szöveg (21 bájt); a típusonkénti `guess` és a tárgy `label` mezője 1 bájtos index ide (0 = nincs) |
| szobák | 9 + MAX_PASSAGES (13) | a folyosórendszerek csak az A változatban ([1.4.7](#147-kijáratok-és-folyosók)) |
| kijáratok | 26 | közös ajtótábla a szint összes kijáratáról ([1.4.7](#147-kijáratok-és-folyosók)); nem telhet be |

**Betelt tábla:**
- *Szörny:* ha nincs szabad hely, az új szörny nem jön létre. A vándorló szörny démon ilyenkor úgy viselkedik, mintha létrehozta volna (az időzítő újraindul); a create monster tekercs „you hear a faint cry of anguish in the distance”; a kincses szoba a további szörnyeket kihagyja. A polymorph a meglévő rekordot használja, nem foglal újat.
- *Tárgy:* a generálás (szint tárgyai, kincses szoba, szobák aranya, szörnyek kezdő tárgya, leprechaun aranya) csak akkor hoz létre tárgyat, ha legalább ITEM_RESERVE = 8 hely szabad; a tartalék a kötegek darabolásának (dobás, lerakás) jut. Ha egyetlen hely sincs, a darabolással járó parancs nem hajtódik végre: „there's no room for that” (nem visz időt).
- *Név:* ha nincs szabad hely, és az elnevezés új helyet foglalna, a `c` és az ismeretlen tárgy használata utáni kérdés helyett „you have no room for more names. Remove a name from something first” jelenik meg ([4.2.6](04_jatekciklus_es_parancsok.md#426-tárgyválasztás-listák-és-tárgyparancsok)). Átnevezés nem foglal új helyet. Egy hely felszabadul:
  - ha a játékos üres nevet ad meg („you remove the name”);
  - a típus azonosításakor, mert ekkor a név úgyis törlődik;
  - ha a címkézett tárgy megszűnik: elvész vagy eltörik dobáskor, a padlón marad szintváltáskor, a nimfa ellopja, vagy győzelemkor eladódik.
- A korlátok úgy vannak méretezve, hogy rendes játékban ne teljenek be; a fejlesztői mód kiírja a foglaltságukat.

### 1.4.2 Számok
| mező | ábrázolás |
|---|---|
| `seed` | 32 bit előjel nélküli; a fejlesztői képernyőn előjelesen jelenik meg ([10.5](10_platform.md#105-aritmetika)) |
| `exp` (játékos és szörny), `gold`, pontszám, tárgyérték | 32 bit előjel nélküli; telítődik 16 777 215-nél (így a ranglistafájlban 3 bájton elfér) és 0-nál |
| `hpt`, `maxhp`, `food_left`, időzítők, `wander_timer`, az arany mennyisége (Item `arm`), `gold_val`, `dist2`, `scroll_title_seed` | 16 bit (a `food_left`, a `hpt` és a `maxhp` előjeles) |
| `str`, `str_max`, `lvl`, `level`, `max_level`, `count`, `group`, a pálca töltése (Item `arm`), `no_command`, `vf_hit`, `count_prefix`, `regen_counter`, `wander_check_tick`, `no_food`, `hungry_state` | 8 bit előjel nélküli (`vf_hit` és `no_food` 255-nél telítődik) |
| Creature `arm`, Item `hplus`, `dplus`, a páncél védelme és a gyűrű modja (Item `arm`) | 8 bit előjeles (szörnynél a mélységi módosító után −128-nál telítődik) |
| koordináták | 8 bit (x 0..79, y 0..23) |

### 1.4.3 Sebzés-stringek
Egy sebzés-string legfeljebb 3 szegmens; minden szegmens egy (N, M) bájtpár. A típustáblákban (6.1, 8.4) így tárolódik; a tárgyak sebzése egyszegmensű, egy-egy (N, M) pár a fegyvertáblában, illetve a botnál és a pálcánál rögzítve (8.7); a tárgyrekordban nem tárolódik ([1.4.6](#146-tárgyrekord)).

### 1.4.4 Szövegek
| szöveg | legfeljebb |
|---|---|
| játékosnév (`name`), gyümölcsnév (`fruit`) | 15 karakter |
| elnevezés (`guess`, `label`) | 20 karakter |
| mentésfájl neve (`file`) | 16 karakter (CBM DOS) |
| tekercsfelirat | 30 karakter (az eredetiben 40, [8.8](08_targyak.md#88-azonosítás-és-nevek)) |

**Tárolás:** minden játékszöveg egy helyen van, névvel ellátott, nullával lezárt ASCII-bájtsorként, számozott táblázat nélkül. A változó részek helyén egybájtos helyőrző áll (`\1`, `\2`: szöveg; `\3`: szám), a 128–255 bájtértékek szabadok. A `message_format(szöveg, első, második, szám)` a `\1` helyére az első, a `\2` helyére a második szöveget, a `\3` helyére a számot (16 bites, előjeles) írja; egy helyőrző többször is előfordulhat. A nagybetűs kezdés és a záró pont a kitöltött szövegre vonatkozik (3.4). A fordító a literálokat ASCII-ban hagyja (nincs PETSCII-átalakítás), ezt az önteszt ellenőrzi. A külön szövegváltozatot (hallucinálva) a hívás helye választja ki.

**Elhelyezés:** a szövegek a fő régión kívül, a `$D000–$FFF9` alatti RAM-ban vannak, és kiíráskor egy pufferbe másolódnak ([10.2](10_platform.md#102-memória)). A címképernyő, a győzelmi képernyő, a sírkő és a ranglista szövegei a képernyőmodulban vannak ([10.10](10_platform.md#1010-képernyőmodul)).

A beviteli mezőkben a korlát elérésekor további karakter nem írható be. Az azonosítatlan nevek (`names`) a színek, kövek és anyagok esetében táblaindexek; a tekercsfeliratok nem tárolódnak, minden kiíráskor a `scroll_title_seed`-ből állnak elő ([8.8](08_targyak.md#88-azonosítás-és-nevek)).

### 1.4.5 Térképrétegek
| réteg | méret | tartalom |
|---|---|---|
| MAP karakter | 80 × 22 bájt | a valódi mező jele |
| MAP flagek | 80 × 22 bájt | 1.2.4 |
| SCREEN (a kódban `game.seen_map`) | 80 × 22 bájt | a játékos által látott jel, 7. bit = inverz |

A rétegek indexe `(y − 1) × 80 + x`. A **teljes perzisztens játékállapot** (GameState, táblák, a MAP karakter-, a MAP flag- és a SCREEN réteg, seed) egyetlen összefüggő memóriaterületen van, mert a mentés ezt egy darabban írja ki ([10.7](10_platform.md#107-lemezkezelés)). Becsült mérete kb. 8,5 KB. A beállítások külön blokkban vannak, a mentés azt is kiírja ([10.10](10_platform.md#1010-képernyőmodul)).

A **„szörny a mezőn?”** kérdésre az F_MONST flag felel: ha nincs beállítva, azonnal „nincs”; ha be van állítva, a mezőn álló szörnyet a szörnylistából keressük ki (legfeljebb 40 elem). Ez csak ott fordul elő, ahol tényleg szörny áll. Külön szörnyréteg nincs.
- **Karbantartás:** minden szörny-létrehozás, -mozgás (`relocate`, teleport) és -törlés beállítja, illetve törli a régi és az új mezőn. A polymorph nem változtat rajta, mert a szörny helyben marad.
- **Invariáns (M11-02):** az F_MONST bitnek mindig egyeznie kell a szörnylistával: pontosan azokon a mezőkön van beállítva, ahol egy szörny áll.
- **Betöltés:** a bit a flagekkel együtt a mentésbe kerül, de betöltés után a biztonság kedvéért újraépül: minden mezőn törlődik, majd a szörnylista minden elemének mezőjén beállítódik.

### 1.4.6 Tárgyrekord
A logikai Item ([1.1.3](#113-item)) mezői közül csak az tárolódik, ami tárgyanként eltérhet; ami a típusból jön, azt a kód a típustáblából veszi. Egy rekord 11 bájt:

| mező | bájt | tartalom |
|---|---|---|
| `type` | 1 | kategória |
| `which` | 1 | altípus |
| `location` | 2 | a `pos` és a `packch` közös helye: padlón a `floor_position` (`x`, `y`); csomagban a `pack_letter` (`'a'..'z'`); szörnynél nincs jelentése. Hogy melyik érvényes, azt az dönti el, melyik listában van a tárgy |
| `count` | 1 | darabszám |
| `properties` | 2 | a `hplus`, a `dplus` és az `arm` közös helye, típusonként: fegyvernél a `weapon.hit_bonus` (`hplus`) és a `weapon.damage_bonus` (`dplus`), 1 + 1 bájt, előjeles; páncélnál az `armor_class`, gyűrűnél a `ring_modifier` (1 bájt, előjeles); pálcánál a `charges` (1 bájt); aranynál a `gold_amount` (16 bit). A név azért nem `value`, mert az „érték” a játékban a tárgy eladási ára |
| `flags` | 1 | ISCURSED, ISKNOW, ISFOUND, ISPROT |
| `group` | 1 | kötegazonosító ([8.4](08_targyak.md#84-fegyverek)) |
| `label` | 1 | a név indexe a névkészletben (0 = nincs) |
| `next` | 1 | a lista következő rekordja (0 = vége) |

Nem tárolódik: a `launch`, a `damage`, a `hurldmg`, valamint az ISMISL és az ISMANY flag, mert ezek a típusból adódnak.

### 1.4.7 Kijáratok és folyosók
Két változat van, a szörnymozgás megírásakor döntünk, melyik marad.

**Mindkettőben közös: ajtótábla.** A szobánkénti fix kijáratlisták helyett egyetlen tábla van a szint összes kijáratáról.
- Minden folyosóvég egy bejegyzés, amikor a `door()` lefut ([2.6](02_szintgeneralas.md#26-folyosók)). Labirintusnál is, ahol a kijárat folyosómező, nem ajtó. A fantom szoba végén nincs bejegyzés.
- Egy szinten legfeljebb 13 folyosó van (8 a feszítőfában és legfeljebb 5 extra, [2.4](02_szintgeneralas.md#24-összekötés)), így legfeljebb 26 bejegyzés.
- A sorrend a felvétel sorrendje. Ha két kijárat egyforma távolságra van a céltól, az első nyer.
- Egy szoba kijáratai azok a bejegyzések, amelyeknek a `room` mezője ez a szoba.

**A változat: folyosórendszerekkel (az eredeti szabály)**
- A generálás végén a `passnum` megszámozza az összefüggő folyosóhálózatokat (F_PNUM). Minden hálózat egy „szoba” rekord a `passages[]` táblában, PHANTOM és DARK flaggel, legfeljebb MAX_PASSAGES = 13.
- Bejegyzés: `position` (2 bájt), `room` (1), `passage` (1), azaz a hálózat, amelyhez a kijárat csatlakozik. Egy hálózat kijáratai azok a bejegyzések, amelyeknek a `passage` mezője ez a hálózat.
- A szörnymozgás ([6.6](06_szornyek.md#66-mozgás)), a sötétség ([3.1](03_latoter_es_megjelenites.md#31-felderítés)) és a drain life ([8.7](08_targyak.md#87-pálcákbotok-z)) úgy működik, ahogy ott le van írva.
- Méret: 22 rekord × 9 bájt + 26 × 4 bájt = **302 bájt**.

**B változat: folyosórendszerek nélkül**
- Nincs `passages[]` és nincs `passnum`. Folyosómezőn az F_PNUM bitjei nincsenek használatban.
- Bejegyzés: `position` (2 bájt) és `room` (1).
- A lény `room` mezője a szoba indexe, vagy „folyosón”. Folyosón az áll, aki F_PASS mezőn van, és az nem labirintus.
- **A szörny mozgása folyosón:**
  - *Ha a cél a játékos, és ő is folyosón áll:* a szokásos `chase` a játékos felé, visszafordulni is szabad. Ha a játékos egy másik folyosón van, a szörny beragadhat, ez belefér.
  - *Különben előre halad.* Jelöltek a szomszédos folyosó- és ajtómezők, amelyek a `diag_ok` és a `step_ok` szerint léphetők, kivéve azt, amelyről a szörny most jött. Ha több jelölt van (elágazás, érintkező folyosók), a célhoz `dist2` szerint legközelebbit választja; egyenlőségnél egyenletesen, `rnd(++ties)`. Ha egy jelölt sem léphető, például mert szörny áll ott, helyben marad.
  - *Ha nincs eltárolt irány* (folyosón született, teleportált), minden szomszéd jelölt.
  - *Ajtóra érve* belép a szobába, onnan a szobaszabály érvényes: a célhoz legközelebbi kijárat.
  - Ehhez szörnyenként el kell tárolni az utolsó lépés irányát. Ez 3 bit, ha elfér a szörny flagjei között, különben 1 bájt.
- **Labirintusban** a szobaszabály érvényes, a labirintus kijárataival. Ha a szörny beragad, az belefér.
- **Sötétség:** folyosón a játékos csak a 3×3 környezetét látja, ahogy most is. Ehhez elég a „folyosón” állapot.
- **Drain life folyosón:** a varázslás pillanatában egy kitöltés a játékos mezőjéről az összefüggő F_PASS mezőkön, a labirintussal együtt. A bejárt mezők jelölésére a folyosómezők szabad F_PNUM bitjei szolgálnak, a végén törlődnek. A célpontok: a bejárt mezőkön állók és a szomszédos ajtókon állók. Ez ugyanaz a kör, mint az A változatban.
- Az eredetitől ebben tér el: folyosón a szörnyek akkor sem ragadnak be, ha a hálózat elágazik, vagy kerülővel vezet. A játék emiatt kicsit nehezebb lehet. Ha ez a változat marad, C64-eltérésként felkerül az eltérések jegyzékébe ([0.3](00_attekintes.md#03-eltérések-és-javítások-jegyzéke)).
- Méret: 9 rekord × 9 bájt + 26 × 3 bájt = **159 bájt**, szörnyenként 1 bájt iránnyal **199 bájt**.
