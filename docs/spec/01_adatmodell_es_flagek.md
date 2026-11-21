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
  // a szoba aranya tárgyként fekszik a padlón, ROOMGOLD jelzővel (1.2.3)
}
// összekötéshez (csak generálás közben): conn[9][9] szomszédsági tábla, isconn[9][9], ingraph[9]
// folyosórendszerek: passages[] – minden összefüggő folyosóhálózat külön "szobaként" szerepel
//   (flags = PHANTOM|DARK), ez kell a drain life-hoz és a szörnyek útkereséséhez; legfeljebb MAX_PASSAGES (1.4.7)
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
  room      // a szoba vagy folyosórendszer, amelyben áll (1.1.1, 1.4.7)
  flags     // állapotflagek (1.2.1 játékos, 1.2.2 szörny)
  pack      // list<Item>: játékosnál a csomag, szörnynél a nála lévő tárgyak
  // csak szörny:
  oldch     // a mező képernyőjele a szörny alatt; ezt kell visszarajzolni, ha elmegy vagy nem látszik
            // (a játékosnál nincs: az ő régi mezőjére a floor_at kerül, 3.1)
  type      // 'A'..'Z': a típus betűje, index a típustáblába (6.1)
  dest      // hová tart: a játékos vagy egy padlón fekvő tárgy (6.5)
  disguise  // amilyen jellel kirajzolódik: a saját betűje, a Xeroc álcázva tárgyjel (6.2, 6.7)
  turn      // bool: ISSLOW szörnynél váltogat, csak true-nál lép (6.2); a C64-en egy flagbit (1.2.2)
}
// a szörny neve és carry értéke a típustáblából jön (6.1), nem kell a rekordban tárolni
// a Venus flytrap szorítása (flytrap_grip) nem a lényhez tartozik: globális, a GameState-ben van
```
Játékos-specifikus állapot a `GameState`-ben (lásd lent). A C64-en a játékos és a szörny rekordja külön van, a szörnyét az [1.4.8](#148-szörnyrekord) írja le.

### 1.1.3 Item
```
Item {
  type     // kategória: POTION, SCROLL, FOOD, WEAPON, ARMOR, STICK, RING, GOLD, AMULET
  which    // altípus index a kategórián belül (a 8.2–8.7 táblák # oszlopa); ételnél 0 = ration, 1 = gyümölcs
  pos      // pozíció, ha a padlón van
  packch   // csomagbetű ('a'..'z'), ha a csomagban van (8.9)
  launch   // a kilövő fegyver típusa, amellyel lőni kell (nyílnál: short bow); nincs: üres (7.4) – nem tárolt, a típusból jön
  damage   // sebzés-string közelharcban, kézben tartva ("NxM") – nem tárolt: fegyvernél a típusból (8.4), pálcánál az anyagából: bot vagy pálca (8.7, 8.8)
  hurldmg  // sebzés-string dobva vagy kilőve (7.4) – nem tárolt, mint a damage
  count    // darabszám kötegelt tárgynál (nyíl, dart, shuriken, dagger, bájital, tekercs, étel)
  hplus    // találati bónusz (fegyvernél; átkozott fegyvernél negatív) (7.1, 8.4)
  dplus    // sebzésbónusz (fegyvernél) (7.1, 8.4)
  arm      // jelentése típusonként: páncélnál védelem (kisebb = jobb); pálcánál töltés; aranynál az összeg; gyűrűnél a mod (pl. protection +1)
  flags    // ISCURSED, ISKNOW, ISMISL, ISMANY, ISFOUND, ISPROT, ROOMGOLD (1.2.3); az ISMISL és az ISMANY nem tárolt, a típusból jön
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
  level                  // a jelenlegi dungeon-szint (mélység), 1..254 (E16)
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
  flytrap_grip           // a Venus flytrap szorítása (eredetileg vf_hit): a flytrap mellé ütése ennyit sebez; > 0, amíg a játékos fogva van (6.7, J4)
  has_amulet             // a játékosnál van-e az amulett (8.10)
  str_max                // a játékos legnagyobb elért ereje gyűrűk nélkül; a státuszsoron "Str:cur(max)" (5.6)
  timers: {              // hátralévő körök az egyes hatásokból (4.1.1)
    confused             // zavarodottság (ISHUH)
    hallucinating        // hallucináció (ISHALU)
    see_invis            // láthatatlan-látás a bájitaltól (CANSEE, J5)
    blind                // vakság (ISBLIND)
    levitating           // lebegés (ISLEVIT)
    hasted               // gyorsítás (ISHASTE)
    turns_unable_to_act  // alvás/ájulás/fagyás: ennyi körig nem cselekedhet a játékos (eredetileg no_command)
    bear_trap            // ennyi körig nem léphet el a medvecsapdából (9.1)
    monster_detect       // szörnyérzékelés (SEEMONST)
  }
  wander_timer           // időzítő a vándorló szörny démon indulásáig (6.2)
  wander_check_tick      // a vándorló démon körszámlálója (between, 6.2)
  know[type][which]      // a bájital, a tekercs, a gyűrű és a pálca altípusainak ismerete, típusonként egy tömbben (8.2, 8.3):
                         // 0 = ismeretlen; 1..MAX_NAMES = ismeretlen, a játékos nevet adott neki ('c'),
                         // index a névkészletbe (1.4.1); 0xff = azonosítva. Egy érték elég, mert azonosításkor a név törlődik,
                         // azonosított típus pedig nem nevezhető el
  names[type][which]     // azonosítatlan nevek ebben a játékban, típusonként egy tömbben: színek, kövek, anyagok (8.8); a pálcánál az index a közös anyaglistába mutat,
                         // ebből tudható, hogy bot (staff) vagy pálca (wand)
  scroll_title_seed      // ebből állnak elő a tekercsfeliratok minden kiíráskor; a feliratok nem tárolódnak (8.8)
  rooms[9], passages[]   // a 9 szoba és a folyosórendszerek; C64-en egy tábla, a rendszerek a szobák után (1.1.1, 1.4.7)
  exits                  // a közös ajtótábla (1.4.7)
  monsters               // a szinten lévő szörnyek listája
  floor_items            // a padlón lévő tárgyak listája
  last_command           // az előző parancs, az 'a' ismétléshez (4.2.5)
  count_prefix           // a parancs előtti számprefix (pl. "5s") (4.2.5)
  can_huh                // CANHUH: a következő sikeres ütés összezavarja a szörnyet (8.3)
  // az 'f'-harc állapota (iránya, kamikaze-e, max_hit_this_fight, 7.2) nincs itt: a harc alatt nincs parancsolvasás, így mentés sem
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
| ISRUN | „ébren van” – egyetlen hatása: ha hiányzik, a szörnyek +4-gyel találják el (a cselekvőképességet a `turns_unable_to_act` számláló kezeli). Pontosan akkor hiányzik, amikor `turns_unable_to_act > 0` (J1), ezért nem kell tárolni | a játék kezdetén; ájulás/alvás/fagyás lejártakor | alvás (tekercs, gáz), fagyás, gyorsítás-ájulás (ha legalább 1 kör), éhség-ájulás |
| ISHUH | zavarodott: mozgásnál 80% véletlen irány (`rndmove`, [6.6](06_szornyek.md#66-mozgás): előbb `dy`, aztán `dx = rnd(3)-1`; (0,0) nem visz időt), dobásnál/`^`-nál 20% véletlen irány | bájital, Medusa | időzítő; időtartamok összeadódnak |
| CANHUH | a következő sikeres ütés összezavarja a szörnyet | Monster confusion tekercs | első sikeres ütés (közel vagy dobás) |
| CANSEE | látja a láthatatlant; **C64-javítás (J5):** akkor áll fenn, ha a see invisible időzítő fut *vagy* see invisible gyűrű van valamelyik kézen | bájital, gyűrű | ha egyik forrás sincs már |
| ISBLIND | vak: térkép nem frissül | bájital | időzítő, gyógyítás |
| ISLEVIT | lebeg: csapdák hatástalanok, nem ébreszt, nem lépcsőzhet, nem vehet fel | bájital | időzítő |
| ISHALU | hallucinál: szörnyek/tárgyak véletlen jellel, minden körben újrarajzolva | bájital | időzítő, extra gyógyítás |
| SEEMONST | szörnyek látszanak falon át (inverz), ISINVIS nem | bájital (20 kör) | időzítő |
| ISHASTE | 2 akció / kör | bájital | időzítő |
| ISHELD | nem léphet el a mezőről, csak a flytrapot támadhatja meg lépéssel (harc, tárgyhasználat mehet). Pontosan akkor áll fenn, amikor `flytrap_grip > 0` (J4), ezért nem kell tárolni | Venus flytrap | flytrap halála, teleport, új szint, egyes pálcák flytrapra; mindegyik a `flytrap_grip`-et is nullázza |

*C64-javítás (J1):* az eredetiben a játékos ISRUN nélkül kezdett, és az éhség-állapotváltás, valamint a 0 körös gyorsítás-ájulás is törölte ([5.4](05_jatekos.md#54-éhség)).

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
| `turn` | nem flag az eredetiben, csak a C64-en: lassított szörnynél ebben a körben cselekszik ([1.1.2](#112-creature-játékos-és-szörny-közös-rekord)) |

A flagek 16 biten férnek el; a típustábla flagjei (ISMEAN, ISFLY, ISGREED, ISINVIS) az alsó bájtban vannak, mert a típustábla ([6.1](06_szornyek.md#61-típusok)) egy bájton tárolja őket.

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
| ROOMGOLD | a szoba aranya: a szoba berendezésekor lerakott arany ([2.7](02_szintgeneralas.md#27-arany-és-kezdő-szörnyek-szobánként-nem-fantom-szoba)). A kapzsi szörny erre megy, és nem veszi fel, hanem ráül ([6.4](06_szornyek.md#64-ébresztés)–[6.6](06_szornyek.md#66-mozgás), E23) |

### 1.2.4 Mező
| flag | jelentés |
|---|---|
| F_PASS | folyosó vagy labirintus mező |
| F_SEEN | a játékos már látta a mezőt |
| F_REAL | nem rejtett („amit látsz, az van ott”); ha hiányzik, a mező rejtett (lásd lent) |
| F_PNUM | a folyosórendszer sorszáma (alsó 4 bit, 1-től), folyosó-, labirintus- és ajtómezőn ([1.4.7](#147-kijáratok-és-folyosók)) |
| F_TMASK | a csapda típusa (alsó 3 bit), csapda-mezőn ([9.1](09_csapdak.md#91-rálépés)) |
| F_MONST | a mezőn szörny áll; hogy melyik, azt a szörnytábla adja meg ([1.4.5](#145-térképrétegek)) |

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

A térkép két rétegű: a **MAP** a valódi szint (karakter és flagek mezőnként; hogy melyik szörny áll egy mezőn, azt az F_MONST flag és a szörnytábla adja meg), a **SCREEN** az, amit a játékos lát és amire emlékszik. Megjelenítéskor a MAP-ből másolunk a SCREEN-re. A SCREEN teljes, 80×22-es réteg a memóriában; a képernyőre ebből csak a nézőablak kerül ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).

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
| CHAR_PASSAGE | `#` | | CHAR_FOOD | `:` |
| CHAR_DOOR | `+` | | CHAR_WEAPON | `)` |
| CHAR_FLOOR | `.` | | CHAR_ARMOR | `]` |
| CHAR_PLAYER | `@` | | CHAR_AMULET | `,` |
| CHAR_TRAP | `^` | | CHAR_RING | `=` |
| CHAR_STAIRS | `%` | | CHAR_STICK | `/` |
| CHAR_GOLD | `*` | | CHAR_MAGIC | `$` (csak mágia-észlelés) |
| CHAR_POTION | `!` | | CHAR_SOLID_ROCK | ` ` |
| CHAR_SCROLL | `?` | | CHAR_MONSTER | `A`..`Z` |
| CHAR_WALL_HORIZONTAL | `-` | | CHAR_WALL_VERTICAL | `\|` |

Nincs külön sarok-karakter: a felső és alsó fal a sarkokkal együtt `-`, az oldalfalak `|`. A kódban minden jel `CHAR_` előtagot kap (`map.h`), mert a tárgykategóriák (pl. `POTION`, `ARMOR`, [1.1.3](#113-item)) ugyanezeket a neveket használják; az álkódokban a rövid név (pl. `FLOOR`) térképjelnél a `CHAR_` konstanst jelenti.

A jelek **ASCII-kódok**: a saját karakterkészletben a karakter indexe az ASCII-kód (32..126), és a 128-cal eltolt index ugyanannak az inverz változata ([10.3](10_platform.md#103-képernyő-és-karakterkészlet)). Így a MAP, a SCREEN és a videomemória ugyanazt a kódot tárolja, a szövegek is ASCII-ban vannak (PETSCII-átalakítás csak a lemezes fájlneveknél kell, [10.7](10_platform.md#107-lemezkezelés)). A SCREEN rétegben a 7. bit az inverz megjelenítést jelzi (SEEMONST).

---

## 1.4 Ábrázolás C64-en

> **C64-eltérés (E10, E12):** az eredeti dinamikus listái és tetszőleges hosszú szövegei helyett fix méretű táblák és korlátos hosszak.

### 1.4.1 Táblák és listák
Minden rekordfajta egy fix méretű táblában (pool) van; a rekordokra 1 bájtos **index** hivatkozik (0 = nincs). A tárgylisták (padlón lévő tárgyak, csomag, egy szörny csomagja) egyszeresen láncoltak a rekordok `next` indexmezőjén át, így a sorrendjük az eredetié.

**A szörnytábla tömör, szörnylista nincs.** A szörnyek az 1..`monster_count` indexeken állnak, a létrehozás sorrendjében; az új szörny a tábla végére kerül. A szörnyek hátulról előre járódnak be, így a sorrend ugyanaz, mint az eredeti szörnylistáé, amelynek az elejére került az új szörny ([6.2](06_szornyek.md#62-generálás)): a legújabb cselekszik először.
- **Kivétel:** a kivett szörny mögött állók egy hellyel előrecsúsznak, a `monster_count` eggyel csökken.
- **Az indexek eltolódnak**, ezért szörnyindexet semmi nem tárol tartósan; a szörny célja ([6.5](06_szornyek.md#65-célpont)) tárgy vagy a játékos, más szörny soha.
- **Halál a szörnyek köre közben:** a `runners` ([6.6](06_szornyek.md#66-mozgás)) bejárásának indexe egy globális változóban van, amíg a kör tart (különben 0). Ha egy kisebb indexű, még nem cselekedett szörny kerül ki (pl. a sárkány lángja megöli, [7.6](07_harc.md#76-elemi-sugár-lightning--fire--cold-pálca-sárkánytűz)), a kivevő függvény ezt az indexet is eggyel csökkenti, különben a soron lévő szörny kétszer cselekedne. Ha maga a soron lévő vagy egy nagyobb indexű szörny kerül ki, nincs teendő. A szabály így egy helyen van, a hívónak nem kell vele törődnie; halott szörny nem marad a táblában, ezért jelölés és kihagyás sem kell a többi bejárásban.

| tábla | méret | megjegyzés |
|---|---|---|
| szörnyek | MAX_MONSTERS = 40 | 9 kezdő + kincses szoba (≤ 12) + vándorlók és teremtett szörnyek; rekordonként 18 bájt ([1.4.8](#148-szörnyrekord)) |
| tárgyak | MAX_ITEMS = 96 | padló, csomag és szörnycsomagok együtt; az arany is tárgy; rekordonként 11 bájt ([1.4.6](#146-tárgyrekord)); a fejlesztői mód foglaltsága alapján később csökkenthető. A szintgenerálás után legfeljebb 73 hely foglalt (23 a csomagban, 9 arany, 10 kincs, 9 tárgy, 1 amulett, 21 szörnytárgy); 81 alá csökkentve a generálás betöltheti a táblát, ekkor a darabolásnak tartalékot kell hagyni |
| nevek (a típusnév és a `label`) | MAX_NAMES = 32 | közös névkészlet, egy-egy ≤ 20 karakteres szöveg (21 bájt); a típusismeret (`know`) 1..MAX_NAMES értéke és a tárgy `label` mezője 1 bájtos index ide (a `label`-nél 0 = nincs). Fordítási feltétel: MAX_NAMES ≤ 254, mert a `know`-ban a 0 és a 0xff foglalt |
| szobák | 9 + MAX_PASSAGES (12) | a szobák és a folyosórendszerek ([1.4.7](#147-kijáratok-és-folyosók)) |
| kijáratok | MAX_EXITS = 2 × MAX_CORRIDORS = 24 | közös ajtótábla a szint összes kijáratáról ([1.4.7](#147-kijáratok-és-folyosók)); nem telhet be |

**Betelt tábla:**
- *Szörny:* ha nincs szabad hely, az új szörny nem jön létre. A vándorló szörny démon ilyenkor úgy viselkedik, mintha létrehozta volna (az időzítő újraindul); a create monster tekercs „you hear a faint cry of anguish in the distance”; a kincses szoba a további szörnyeket kihagyja. A polymorph a meglévő rekordot használja, nem foglal újat.
- *Tárgy:* új tárgyhoz egy szabad hely kell. Ha nincs, a generálás (szint tárgyai, kincses szoba, szobák aranya, szörnyek kezdő tárgya, leprechaun aranya) kihagyja a tárgyat, a kötegek darabolásával járó parancs (dobás, lerakás) pedig nem hajtódik végre: „there's no room for that” (nem visz időt).
- *Név:* ha nincs szabad hely, és az elnevezés új helyet foglalna, a `c` és az ismeretlen tárgy használata utáni kérdés helyett „you have no room for more names. Remove a name from something first” jelenik meg ([4.2.6](04_jatekciklus_es_parancsok.md#426-tárgyválasztás-listák-és-tárgyparancsok)). Átnevezés nem foglal új helyet. Egy hely felszabadul:
  - ha a játékos üres nevet ad meg („you remove the name”);
  - a típus azonosításakor, mert ekkor a név úgyis törlődik;
  - ha a címkézett tárgy megszűnik: elvész vagy eltörik dobáskor, a padlón marad szintváltáskor, a nimfa ellopja, vagy győzelemkor eladódik.
- A korlátok úgy vannak méretezve, hogy rendes játékban ne teljenek be; a fejlesztői mód kiírja a foglaltságukat.

### 1.4.2 Számok
| mező | ábrázolás |
|---|---|
| `seed` | 32 bit előjel nélküli; a fejlesztői menüben is előjel nélkül jelenik meg és adható meg ([10.5](10_platform.md#105-aritmetika)) |
| a játékos `exp`-je, `gold`, pontszám, tárgyérték | 32 bit előjel nélküli; telítődik 16 777 215-nél (így a ranglistafájlban 3 bájton elfér) és 0-nál |
| a szörny `exp`-je | 16 bit előjel nélküli; csak a generálás írja ([6.2](06_szornyek.md#62-generálás)), a legnagyobb értéke kb. 13 650 (dragon a 254. szint kincses szobájában, amelynek szörnyei a 255. szint szerint jönnek létre) |
| `hpt`, `maxhp`, `food_left`, időzítők, `wander_timer`, az arany mennyisége (Item `arm`), `dist2`, `scroll_title_seed` | 16 bit (a `food_left`, a `hpt` és a `maxhp` előjeles) |
| `str`, `str_max`, `lvl`, `level`, `max_level`, `count`, `group`, a pálca töltése (Item `arm`), `turns_unable_to_act`, `flytrap_grip`, `count_prefix`, `regen_counter`, `wander_check_tick`, `no_food`, `hungry_state` | 8 bit előjel nélküli (`flytrap_grip` és `no_food` 255-nél telítődik) |
| Creature `arm`, Item `hplus`, `dplus`, a páncél védelme és a gyűrű modja (Item `arm`) | 8 bit előjeles (szörnynél a mélységi módosító után −128-nál telítődik) |
| koordináták | 8 bit (x 0..79, y 0..23) |

### 1.4.3 Sebzés-stringek
Egy sebzés-string legfeljebb 3 szegmens; minden szegmens egy (N, M) bájtpár. A szegmensek száma külön tárolódik, mert a (0, 0) is valódi szegmens: az aquator `0x0/0x0` sebzése két támadás, mindkettő külön dob a találatra. A típustáblákban (6.1, 8.4) így tárolódik; a tárgyak sebzése egyszegmensű, egy-egy (N, M) pár a fegyvertáblában, illetve a botnál és a pálcánál rögzítve (8.7); a tárgyrekordban nem tárolódik ([1.4.6](#146-tárgyrekord)).

### 1.4.4 Szövegek
| szöveg | legfeljebb |
|---|---|
| játékosnév (`name`), gyümölcsnév (`fruit`) | 15 karakter |
| elnevezés (típusnév, `label`) | 20 karakter |
| mentésfájl neve (`file`) | 16 karakter (CBM DOS) |
| tekercsfelirat | 30 karakter (az eredetiben 40, [8.8](08_targyak.md#88-azonosítás-és-nevek)) |

**Tárolás:** minden játékszöveg egy helyen van (`texts.c`, kivétel lent az elhelyezésnél), névvel ellátott, nullával lezárt ASCII-bájtsorként, számozott táblázat nélkül. A változó részek helyén egybájtos helyőrző áll (`\1`, `\2`: szöveg; `\3`: szám), a 128–255 bájtértékek szabadok. A `message_format(szöveg, első, második, szám)` a `\1` helyére az első, a `\2` helyére a második szöveget, a `\3` helyére a számot (16 bites, előjeles) írja; egy helyőrző többször is előfordulhat. A nagybetűs kezdés és a záró pont a kitöltött szövegre vonatkozik (3.4). A fordító a literálokat ASCII-ban hagyja (nincs PETSCII-átalakítás), ezt az önteszt ellenőrzi. A külön szövegváltozatot (hallucinálva) a hívás helye választja ki.

**Elhelyezés:** a szövegek a fő régión kívül, a `$D000–$FFF9` alatti RAM-ban vannak, és kiíráskor egy pufferbe másolódnak ([10.2](10_platform.md#102-memória)). A címképernyő, a győzelmi képernyő, a sírkő és a ranglista szövegei a képernyőmodulban vannak ([10.10](10_platform.md#1010-képernyőmodul)). Kivétel: a státuszsor feliratai (`Level:`, `Gold:` stb.) a kiírásuk helyén, a `--More--` makróként (`TEXT_MORE`, a hossza fordításkor kell) a fő régióban maradnak. Kevés helyet foglalnak, és a státuszsor minden körben kirajzolódik, így nem kell érte lapozni.

A beviteli mezőkben a korlát elérésekor további karakter nem írható be. Az azonosítatlan nevek (`names`) a színek, kövek és anyagok esetében táblaindexek; a tekercsfeliratok nem tárolódnak, minden kiíráskor a `scroll_title_seed`-ből állnak elő ([8.8](08_targyak.md#88-azonosítás-és-nevek)).

### 1.4.5 Térképrétegek
| réteg | méret | tartalom |
|---|---|---|
| MAP karakter | 80 × 22 bájt | a valódi mező jele |
| MAP flagek | 80 × 22 bájt | 1.2.4 |
| SCREEN (a kódban `game.seen_map`) | 80 × 22 bájt | a játékos által látott jel, 7. bit = inverz |

A rétegek indexe `(y − 1) × 80 + x`. A **teljes perzisztens játékállapot** (GameState, táblák, a MAP karakter-, a MAP flag- és a SCREEN réteg, seed) egyetlen összefüggő memóriaterületen van, mert a mentés ezt egy darabban írja ki ([10.7](10_platform.md#107-lemezkezelés)). Becsült mérete kb. 8,5 KB. A beállítások külön blokkban vannak, a mentés azt is kiírja ([10.10](10_platform.md#1010-képernyőmodul)).

A **„szörny a mezőn?”** kérdésre az F_MONST flag felel: ha nincs beállítva, azonnal „nincs”; ha be van állítva, a mezőn álló szörnyet a szörnytáblából keressük ki (legfeljebb 40 elem). Ez csak ott fordul elő, ahol tényleg szörny áll. Külön szörnyréteg nincs.
- **Karbantartás:** minden szörny-létrehozás, -mozgás (`relocate`, teleport) és -törlés beállítja, illetve törli a régi és az új mezőn. A polymorph nem változtat rajta, mert a szörny helyben marad.
- **Invariáns (M11-02):** az F_MONST bitnek mindig egyeznie kell a szörnytáblával: pontosan azokon a mezőkön van beállítva, ahol egy szörny áll.
- **Betöltés:** a bit a flagekkel együtt a mentésbe kerül, de betöltés után a biztonság kedvéért újraépül: minden mezőn törlődik, majd a szörnytábla minden szörnyének mezőjén beállítódik.

### 1.4.6 Tárgyrekord
A logikai Item ([1.1.3](#113-item)) mezői közül csak az tárolódik, ami tárgyanként eltérhet; ami a típusból jön, azt a kód a típustáblából veszi. Egy rekord 11 bájt:

| mező | bájt | tartalom |
|---|---|---|
| `type` | 1 | kategória |
| `which` | 1 | altípus |
| `location` | 2 | a `pos` és a `packch` közös helye: padlón a `floor_position` (`x`, `y`); csomagban a `pack_letter` (`'a'..'z'`); szörnynél nincs jelentése. Hogy melyik érvényes, azt az dönti el, melyik listában van a tárgy |
| `count` | 1 | darabszám |
| `properties` | 2 | a `hplus`, a `dplus` és az `arm` közös helye, típusonként: fegyvernél a `weapon.hit_bonus` (`hplus`) és a `weapon.damage_bonus` (`dplus`), 1 + 1 bájt, előjeles; páncélnál az `armor_class`, gyűrűnél a `ring_modifier` (1 bájt, előjeles); pálcánál a `charges` (1 bájt); aranynál a `gold_amount` (16 bit). A név azért nem `value`, mert az „érték” a játékban a tárgy eladási ára |
| `flags` | 1 | ISCURSED, ISKNOW, ISFOUND, ISPROT, ROOMGOLD |
| `group` | 1 | kötegazonosító ([8.4](08_targyak.md#84-fegyverek)) |
| `label` | 1 | a név indexe a névkészletben (0 = nincs) |
| `next` | 1 | a lista következő rekordja (0 = vége) |

Nem tárolódik: a `launch`, a `damage`, a `hurldmg`, valamint az ISMISL és az ISMANY flag, mert ezek a típusból adódnak.

### 1.4.7 Kijáratok és folyosók
**Ajtótábla.** A szobánkénti fix kijáratlisták helyett egyetlen tábla van a szint összes kijáratáról.
- Minden folyosóvég egy bejegyzés, amikor a `door()` lefut ([2.6](02_szintgeneralas.md#26-folyosók)). Labirintusnál is, ahol a kijárat folyosómező, nem ajtó. A fantom szoba végén nincs bejegyzés.
- Egy szinten legfeljebb MAX_CORRIDORS folyosó van: a szomszédos szektorpárok száma (`2 × SECTOR_GRID_SIZE × (SECTOR_GRID_SIZE − 1)`) és a feszítőfa meg az extra folyosók száma (`ROOM_COUNT − 1 + MAX_EXTRA_CORRIDORS`, [2.4](02_szintgeneralas.md#24-összekötés)) közül a kisebb, így legfeljebb MAX_EXITS = 2 × MAX_CORRIDORS bejegyzés. Alapértékekkel 12 és 24.
- A sorrend a felvétel sorrendje. Ha két kijárat egyforma távolságra van a céltól, az első nyer.
- Egy szoba kijáratai azok a bejegyzések, amelyeknek a `room` mezője ez a szoba.

**Folyosórendszerek**
- A generálás végén az `assign_passage_numbers` (eredetileg `passnum`) megszámozza az összefüggő folyosóhálózatokat, a labirintusokkal együtt (F_PNUM, 1-től; [2.4](02_szintgeneralas.md#24-összekötés)). Minden hálózat egy „szoba” rekord PHANTOM és DARK flaggel (az eredetiben a `passages[]` tábla). C64-en ezek a szobatábla végén vannak: az n. hálózat indexe ROOM_COUNT + n − 1, így a lény `room` mezője szobára és folyosórendszerre is ugyanabba a táblába mutat. Fordítási feltétel: a szobatábla (ROOM_COUNT + MAX_PASSAGES) indexe elfér 8 biten, a 0xfe és a 0xff nélkül. Legfeljebb MAX_PASSAGES = MAX_CORRIDORS van, mert mindegyikben van legalább egy folyosó. Fordítási feltétel: a MAX_PASSAGES elfér az F_PNUM 4 bitjén (≤ 15).
- Bejegyzés: `position` (2 bájt), `room` (1), `passage` (1), azaz a hálózat, amelyhez az ajtó csatlakozik. A labirintus kijáratánál a `passage` üres (0): a kijárat maga is a hálózat része, nem a hálózat kijárata. Egy hálózat kijáratai azok a bejegyzések, amelyeknek a `passage` mezője ez a hálózat.
- A szörnymozgás ([6.6](06_szornyek.md#66-mozgás)), a sötétség ([3.1](03_latoter_es_megjelenites.md#31-felderítés)) és a drain life ([8.7](08_targyak.md#87-pálcákbotok-z)) úgy működik, ahogy ott le van írva.
- Méret: (9 + 12) rekord × 5 bájt + 24 × 4 bájt = **201 bájt**.

### 1.4.8 Szörnyrekord
A logikai Creature ([1.1.2](#112-creature-játékos-és-szörny-közös-rekord)) szörnyre vonatkozó mezői közül csak az tárolódik, ami szörnyenként eltérhet. A játékos rekordja külön van, mert a mezők szélessége (pl. az `exp`: a játékosé 32, a szörnyé 16 bit) és köre eltér. Egy szörnyrekord 18 bájt:

| mező | bájt | tartalom |
|---|---|---|
| `x`, `y` | 2 | pozíció |
| `room` | 1 | a szoba vagy a folyosórendszer ([1.4.7](#147-kijáratok-és-folyosók)) |
| `type` | 1 | a típus betűje, `'A'..'Z'` |
| `lvl` | 1 | |
| `arm` | 1 | előjeles |
| `hpt`, `maxhp` | 4 | 2 + 2 bájt, előjeles |
| `exp` | 2 | ennyi XP-t ad ([1.4.2](#142-számok)) |
| `flags` | 2 | [1.2.2](#122-szörny), a `turn` is |
| `oldch` | 1 | a mező képernyőjele a szörny alatt |
| `disguise` | 1 | a kirajzolt jel |
| `dest` | 1 | a padlón fekvő céltárgy indexe; 0 = a játékos |
| `pack` | 1 | a szörny tárgylistájának eleje (0 = nincs) |

Nem tárolódik: az `str` (mindig 10), a `dmg`, a név és a carry érték, mert ezek a típustáblából jönnek ([6.1](06_szornyek.md#61-típusok)).
