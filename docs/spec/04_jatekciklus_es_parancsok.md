# 4. Játékciklus és parancsok

## 4.1 Fő játékciklus

```
új játék (0.1):
  init_identification_tables()  // 8.8
  init_player()                 // 5.1
  level = 1; new_level()
  loop: command()               // a játék végéig (halál, győzelem, kilépés, mentés), utána címképernyő

command():  // egy kör
  // BEFORE fázis
  wander_daemon()                // 6.2 (rollwand)
  if ISHALU: hallucination_redraw()
  BEFORE-időzítők ketyegése
  actions = (player ISHASTE) ? 2 : 1
  repeat actions:
    look(wakeup=true)            // látótér frissítés + szomszédok ébresztése (3.1, 6.4)
    update_status(); draw()
    if no_command > 0:           // alszik / ájult / fagyott
       next_command = '.'        // pihenésnek számít
       if --no_command == 0: player.ISRUN = true; "you can move again"
    else:
       next_command = futás/ismétlés/to_death esetén az előző, különben get_command()
    took_time = execute(next_command)
    if not took_time: actions++  // ingyenes parancs: nem fogy az akció
    ha a lépés tárgyra vitt: pick_up
  // AFTER fázis
  move_monsters()                // 6.6 (runners)
  player_regen()                 // 5.3 (doctor)
  hunger()                       // 5.4 (stomach)
  AFTER-időzítők ketyegése (4.1.1)
  for hand in LEFT, RIGHT:       // gyűrűk (8.6)
    if searching gyűrű: search()
    else if teleportation gyűrű and rnd(50) == 0: teleport()
```
A `waste_time()` ([4.2.3](#423-tárgyak)) a BEFORE és AFTER fázis démonjait/időzítőit futtatja le még egyszer, játékos-akció nélkül.

A démonok és időzítők az eredetiben egy általános démon/fuse-listában vannak. C64-en elég egy rögzített sorrendű hívássor és időzítőnként egy számláló (`GameState.timers`). A BEFORE-időzítők a BEFORE fázisban, az AFTER-időzítők az AFTER fázisban ketyegnek; ha egy fázisban több időzítő is lejár, a 4.1.1 táblázat sorrendjében futnak le (az eredetiben a démonlista-helyek sorrendje döntött, ami csak az üzenetek sorrendjét befolyásolta).

### 4.1.1 Időzített hatások (fuse-ok)
| hatás | időtartam | lejáratkor |
|---|---|---|
| confusion | spread(20) (összeadódik) | ISHUH le, „you feel less confused now” |
| hallucination | spread(850) | ISHALU le, képernyő újrarajzolás |
| see invisible (bájital) | spread(850); ha már fut, spread(850)-nel meghosszabbodik | a bájital-forrás megszűnik; a CANSEE csak akkor kapcsol ki, ha nincs see invisible gyűrű a kézen (J5) |
| blindness | spread(850) | ISBLIND le, képernyő frissítés |
| levitation | spread(30) | ISLEVIT le |
| haste self | rnd(4)+4 | ISHASTE le |
| monster detection | 20 | SEEMONST le, inverz szörnyek eltűnnek |
| sleep (tekercs) | rnd(5)+4 | ISRUN vissza |
| sleeping gas | 5 | ISRUN vissza |
| bear trap | 3 | – |
| ájulás (haste ütközés) | rnd(8) | ISRUN vissza |
| ájulás (éhség) | rnd(8)+4 | ISRUN vissza |
| fagyás (Ice monster) | rnd(2)+2 összeadódik | ISRUN vissza |

> **C64-javítás (J5):** a see invisible bájital és a see invisible gyűrű két független forrás; a CANSEE akkor áll fenn, ha legalább az egyik él. *Eredetileg* az időzítő lejárta a gyűrű ellenére is levette a CANSEE-t; ha iváskor már volt CANSEE (pl. gyűrűtől), nem indult időzítő; a gyűrű levétele pedig a bájital hátralévő hatását is megszüntette.

---

## 4.2 Parancsok

`*` = nem visz el időt. Minden más 1 kör (ha sikeres). Ismeretlen billentyű: „illegal command”.

### 4.2.0 Billentyűzet

> **C64-eltérés (E4):** a parancsok ugyanazok a karakterek, mint az eredetiben, de a C64 billentyűzetén előállítva. A billentyűzetet a program maga olvassa a mátrixból ([10.4](10_platform.md#104-billentyűzet)), nem a KERNAL, mert csak így kezelhető minden SHIFT- és CTRL-kombináció.

| logikai billentyű | C64 |
|---|---|
| `a`–`z` | betűbillentyű |
| `A`–`Z` | SHIFT (vagy SHIFT LOCK) + betű |
| `CTRL-a`–`CTRL-z` | CTRL + betű |
| `0`–`9` | számbillentyű (számprefix) |
| `.` `,` `/` `:` `;` `=` `*` `@` `+` `-` | a saját billentyűjük |
| `>` `<` `?` | SHIFT + `.` / `,` / `/` |
| `)` | SHIFT + `9` |
| `]` | SHIFT + `;` |
| `^` | `↑` |
| ESC | RUN/STOP vagy `←` |
| RETURN, szóköz | RETURN, SPACE |
| törlés (bevitelben) | INST/DEL |

- A C= billentyű, a kurzorbillentyűk, az F-billentyűk, a CLR/HOME és a RESTORE nem csinál semmit (a RESTORE által kiváltott NMI nem szakítja meg a programot).
- **Ismétlés:** a parancsra váró állapotban egy nyomva tartott billentyű 16 képkocka után ismétel, utána 4 képkockánként. A kérdéseknél (`--More--`, igen/nem, irány, tárgybetű, menük) csak új leütés számít.
- **Előre leütött billentyűk:** legfeljebb 8 várakozik egy pufferben. A „billentyűpuffer ürítése” (teleport, csapda) és a `flush` opció ([4.2.5](#425-rendszer)) ezt üríti.
- A beviteli mezőkben a fenti táblázat billentyűi és a SHIFT-tel előállított írásjelek (pl. `!` = SHIFT + `1`, `'` = SHIFT + `7`) gépelhetők.

### 4.2.1 Mozgás
| parancs | leírás |
|---|---|
| h j k l y u b n | lépés; szörnyre lépés = támadás; tárgyra lépve automatikus felvétel |
| m + irány | lépés felvétel nélkül („moved onto …”) |
| SHIFT + irány | futás (lásd lent) |
| CTRL + irány | óvatos futás: ezen felül ajtó mellett, elágazásnál és bármi érdekes mellett is megáll (lásd lent); vakon ugyanaz, mint a SHIFT |
| `>` * | lefelé lépcsőn (ha a lépcsőn áll és nem lebeg) |
| `<` * | felfelé lépcsőn (ha a lépcsőn áll, nem lebeg, és `has_amulet`) |

**Mozgás szabályai:**
```
move(dir):
  if bear_trap > 0: bear_trap--; "you are still stuck in the bear trap"; uses turn
  if ISHUH and rnd(5) != 0:
     target = rndmove(player)                                          // 6.6: ha a véletlen cél nem járható (fal, kő, szabálytalan átló,
     if target == player.pos: no time; futás és 'f'-harc vége; return  //  szörny, scare monster tekercs), helyben marad
  else: target = player.pos + dir                                      // target: a mező, ahová lépni akar
  if target a pályán kívül (0. sor, 23. sor, oszlop < 0 vagy ≥ 80): mint falnál (lent)
  átlós lépés tilos, ha bármelyik ortogonális szomszéd fal/kő (sarokvágás tilalom) – ajtón átlósan sem lehet ki/be lépni; no time
  if target rejtett csapda és !ISLEVIT: felfedődik (TRAP, F_REAL be)
  else if ISHELD and target-en nem a flytrap jele áll: "you are being held"; uses turn
  if monster at target: attack(target)
  else if target walkable (FLOOR, DOOR, PASSAGE, STAIRS, TRAP, tárgy):
     player.pos = target; check room enter/leave (3.1, 6.4); follow_view() (3.5)
     if TRAP: trigger (9)
     if item and not 'm' and not ISLEVIT: pick_up
  else: no time
```
A zavarodott véletlen lépés soha nem támad szörnyre (a szörny jele nem járható), és Scare monster tekercsre sem lép. A „pálya széle” a logikai 80×22-es térkép széle, nem a nézőablaké.

**Futás:** a futás ugyanazt a lépést ismétli minden akcióban, amíg valami le nem állítja.

*Mindkét futást megállítja:*
- fal, kő vagy a pálya széle (kivéve a kanyarkövetést, lent), szabálytalan átlós lépés;
- ajtóra lépés, lépcsőre, tárgyra vagy szörnyre lépés (szörnynél harc indul);
- csapdára lépés, teleport;
- ha egy szörny megtámadja a játékost, vagy a sárkány tüzet okád;
- éhség-állapotváltás ([5.4](05_jatekos.md#54-éhség));
- ha a keresés (searching gyűrű) talál valamit;
- confusion-nál, ha a véletlen irány helyben hagyná.

*Kanyarkövetés (`passgo` opció, alapból KI):* folyosón, ha a futás falba ütközne és pontosan egy irányba lehet derékszögben továbbmenni, a futás befordul. Csak ortogonális futásnál, vakságban nem.

*CTRL-futás (`door_stop`) – minden lépés után megnézi a 3×3 környezetet, a futás irányával ellentétes oldalt kihagyva:*
- egy ajtó a futás vonalában (ugyanabban a sorban/oszlopban) → megáll;
- több mint egy folyosó-mező oldalirányban (elágazás) → megáll;
- bármi más, ami nem padló, fal vagy üres (tárgy, lépcső, szörny, csapda) → megáll;
- az első lépés után kezd ellenőrizni (az indulóhelyet nem nézi).

*A SHIFT-futást egy szörny megjelenése önmagában nem állítja meg*, csak a támadása.

### 4.2.2 Harc és célzás
| parancs | leírás |
|---|---|
| f + irány | harc a célponttal, amíg: valaki meg nem hal, a célpont elmozdul, más szörny támad, vagy `hpt <= max_hit_this_fight`. Csak látott (vagy SEEMONST-tal érzékelt) szörnyre, különben „I see no monster there” (nincs idő). Szabálytalan átlós célnál semmi nem történik, és nem telik idő (J15). Nincsenek hit/miss üzenetek. |
| SHIFT-f + irány | ugyanez, HP-ellenőrzés nélkül („kamikaze”) |
| t + irány | hajítás ([7.4](07_harc.md#74-hajítás-t)) |
| z + irány | pálca/bot használata ([8.7](08_targyak.md#87-pálcákbotok-z)) |

> **C64-javítás (J15):** *eredetileg* a szabálytalan átlós irányba indított `f` nem csinált semmit, de a kör elment.

### 4.2.3 Tárgyak
| parancs | leírás |
|---|---|
| `,` | felvétel (lebegve nem) |
| d | lerakás – csak üres FLOOR vagy PASSAGE mezőre; használatban lévő átkozottat nem |
| q | bájital ivás |
| r | tekercs olvasás |
| e | evés |
| w | fegyver kézbe (átkozott jelenlegi fegyvert nem lehet cserélni). Bármilyen nem-páncél tárgy kézbe vehető (bájital, étel …), csak a páncélt utasítja el |
| SHIFT-w | páncél felvétele – 2 kör (`waste_time`); a páncél ezzel azonosítódik (ISKNOW). Ha már van rajta páncél: „you are already wearing some. You'll have to take it off first”, nem visz időt. Nem páncélra: „you can't wear that” (időbe kerül) |
| SHIFT-t | páncél levétele – 2 kör (`waste_time`); átkozottat nem: „you can't. It appears to be cursed”. Ha nincs rajta: „you aren't wearing any armor”, nem visz időt |
| SHIFT-p | gyűrű felhúzása (melyik kézre kérdez, ha mindkettő szabad) |
| SHIFT-r | gyűrű levétele (átkozottat nem) |
| c * | tárgytípus elnevezése (bájital, tekercs, gyűrű, pálca) |

**Időveszteség (`waste_time`):** a parancs saját köre előtt még egy teljes „üres” kör lefut, amelyben a játékos nem cselekszik, de minden más igen: a szörnyek lépnek és támadnak, fogy az étel, regenerál a HP, ketyegnek az időzítők (a [4.1](#41-fő-játékciklus) BEFORE és AFTER fázisa). A gyűrűhatások (searching, teleportation) ebben az üres körben **nem** futnak le. A páncél felvétele és levétele így összesen 2 körbe kerül. Ugyanez történik, ha a viselt páncélt más parancs veszi le automatikusan, mert lerakod (`d`) vagy eldobod (`t`).
```
waste_time():
  BEFORE-démonok (vándorló-szörny dobás, hallucináció); BEFORE-időzítők
  AFTER-démonok (szörnymozgás, regeneráció, éhség); AFTER-időzítők
```

### 4.2.4 Információ (mind *)
| parancs | leírás |
|---|---|
| i | teljes csomag listája ([4.2.6](#426-tárgyválasztás-listák-és-tárgyparancsok)) |
| SHIFT-i | egy tárgy betű alapján az üzenetsávba; üres csomag: „you aren't carrying anything”; 1 tárgy → azonnal; rossz betű: „'x' not in pack” |
| `)` `]` `=` | kézben lévő fegyver / viselt páncél / gyűrűk; „you are wielding nothing” / „wearing nothing” |
| `@` | a státuszsor kiírása az üzenetsávba |
| SHIFT-d | felfedezett típusok listája ([4.2.6](#426-tárgyválasztás-listák-és-tárgyparancsok)) |
| `/` | jel magyarázata ([4.2.6](#426-tárgyválasztás-listák-és-tárgyparancsok)) |
| `^` + irány | csapda azonosítása a szomszéd mezőn („You have found <csapda>”), rejtettnél vagy ha nincs: „You have found no trap there” (terse: csak „no trap there”). ISHUH-nál 20% eséllyel más irány (mint minden irányos parancsnál, `get_dir`); ISHALU alatt véletlen csapdanév |
| CTRL-p | utolsó üzenet |
| `?` | súgó ([4.2.6](#426-tárgyválasztás-listák-és-tárgyparancsok)) |
| v | verzió („version 5.4.4. (mctesq was here)”) |

### 4.2.5 Rendszer
| parancs | leírás |
|---|---|
| `.` | pihenés 1 kör |
| s | keresés ([9.2](09_csapdak.md#92-keresés-s-searching-gyűrű)) |
| a | utolsó parancs ismétlése; „you haven't typed a command yet” |
| szám + parancs | ismétlés ≤ 255-ször. Megmarad: `.` `a` `s` `t` `q` `r` `z` `m`, a mozgások (kis- és nagybetű, CTRL), `C`, `I`; minden másnál a szám elvész. Megszakítja (a futással azonos esetek): szörnytámadás, sárkánytűz, éhség-állapotváltás, csapda, teleport, sikeres keresés, illegális parancs |
| ESC * | megszakít |
| o * | beállítások (lásd lent) |
| SHIFT-s * | mentés (lásd lent); sikeres mentés után vissza a címképernyőre |
| SHIFT-q * | kilépés: „really quit?” – `y`-ra „You quit with <n> gold pieces”, pontszám = arany (levonás nélkül) |
| CTRL-r * | újrarajzolás (`center_view`, státuszsor) |
| szóköz * | nem csinál semmit |

> **C64-eltérés (E9):** az eredeti `!` (alshell indítása) nincs; a `!` billentyű „illegal command”.

**Beállítások (`o`):**

> **C64-eltérés (E6):** a `ROGUEOPTS` környezeti változó helyett a beállítások a címképernyőről és a játékból (`o`) érhetők el, és a lemezre menthetők (`rogue.cfg`). Új opció a `device`. Az `inven` alapértéke *clear*; a baudsebességtől függő alapértékek elmaradnak.

| opció | alapérték | sor a menüben | jelentés |
|---|---|---|---|
| terse | ki | Terse output ("terse") | rövidebb üzenetek; a dobott tárgy repülése nem animálódik |
| flush | ki | Flush typeahead ("flush") | harcban eldobja az előre leütött billentyűket |
| jump | ki | Jump to end of run ("jump") | futás közben nem rajzol minden lépést, csak a végén ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)) |
| seefloor | be | Show lamp floor ("seefloor") | sötét szobában a 3×3 környezet padlója látszik ([3.1](03_latoter_es_megjelenites.md#31-felderítés)) |
| passgo | ki | Follow corridors ("passgo") | futásnál a folyosó kanyarjait követi ([4.2.1](#421-mozgás)) |
| tombstone | be | Tombstone at death ("tombstone") | halálkor sírkő-rajz ([4.3](#43-szintváltás-és-játék-vége)) |
| inven | clear | Inventory style ("inven") | csomaglista: overwrite (a térkép fölé), slow (soronként az üzenetsávban), clear (külön képernyő) |
| name | a megadott név (0.1) | Name ("name") | a sírkövön és a ranglistán; ≤ 15 karakter |
| fruit | slime-mold | Fruit ("fruit") | a gyümölcs neve; ≤ 15 karakter |
| file | rogue.sav | Save file ("file") | a mentésfájl neve; ≤ 16 karakter |
| device | 8 (ha a program más egységről töltődött be, az) | Disk device ("device") | a lemezegység száma, 8..11 |

- A menü egy teljes képernyő, soronként `<sor a menüben>: <érték>` (logikai: True/False). A kurzor sorról sorra lép: logikai opciónál `t`/`f` állít, inven-nél `o`/`s`/`c`, device-nál a szám, szöveges opciónál a szöveg szerkeszthető ([3.4](03_latoter_es_megjelenites.md#34-üzenetsor) bevitel); RETURN megtartja/elfogadja és a következőre lép, `-` az előzőre lép vissza, ESC befejezi. Bekéréskor a sor végén „(T or F)”, illetve „(O, S, or C)” áll. A végén „--Press space to continue--”.
- A menü elhagyásakor: „save options to disk? ” – `y`-ra a `rogue.cfg` fájl (az opciók és a név) a `device` egységre íródik ([10.7](10_platform.md#107-lemezkezelés)); lemezhiba esetén „disk error: <kód>,<szöveg>”.
- A `rogue.cfg` induláskor betöltődik ([0.1](00_attekintes.md#01-indítás)).

**Mentésfájl:**

> **C64-eltérés (E7):** a mentés lemezre kerül; a szimbolikus link-, a jogosultság- (0400), a képernyőméret-ellenőrzés és a szignálra (SIGHUP) történő automatikus mentés elmarad.

```
save_game():   // SHIFT-s
  "save file (<file>)? "                // y: a file opció neve; n: "file name: " bekérése; más: "please answer Y or N"
     ESC a fájlnév bekérésekor: a mentés elmarad, a játék folytatódik
  ha a fájl már létezik a lemezen: "File exists.  Do you wish to overwrite it?"
     y: a régi fájl törlése; n vagy ESC: vissza a "file name: " kérdéshez; más: "Please answer Y or N"
  kiírás: fejléc + a teljes állapotblokk (10.7)
  lemezhiba: "disk error: <kód>,<szöveg>"; a játék folytatódik (a félig írt fájl törlődik)
  siker: vissza a címképernyőre

restore_game():   // címképernyő, R
  "file name: " (előtöltve a file opcióval); ESC: vissza a címképernyőre
  megnyitás; lemezhiba (pl. 62,FILE NOT FOUND): "disk error: <kód>,<szöveg>", vissza a címképernyőre
  fejléc ellenőrzése; ha nem ennek a verziónak a mentése, vagy rövidebb a vártnál: "Sorry, saved game is out of date.", vissza
  az állapotblokk beolvasása; a MAP szörnyréteg újraépítése a szörnylistából (1.4.5)
  a fájl törlése a lemezről                     // a "halott"-ellenőrzés ELŐTT, mint az eredetiben
  ha hpt <= 0: "\"He's dead, Jim\"", vissza a címképernyőre
  center_view(), teljes újrarajzolás; "file name: <fájl>"; a főciklus folytatódik
```
- Tartalom: a teljes perzisztens játékállapot ([1.4.5](01_adatmodell_es_flagek.md#145-térképrétegek)): minden globális változó és flag, az opciók, a tárgytípusok neve/ismertsége/elnevezése, a szintek számlálói (`level`, `max_level`, `no_food`, `food_left`, `gold`, `vf_hit`, `seed` …), a térképrétegek (a MAP szörnyréteg kivételével, amely betöltéskor újraépül), a szobák és folyosórendszerek, a szörny- és tárgytáblák a listákkal, a futó időzítők. A formátumot a [10.7](10_platform.md#107-lemezkezelés) írja le.
- A bájtok XOR-ral el vannak rejtve (egyszerű elrejtés, nem biztonsági titkosítás), [10.7](10_platform.md#107-lemezkezelés).
- A betöltés után a fájl törlődik, így ugyanonnan nem lehet újrakezdeni (a lemez másolásától ez nem véd).

### 4.2.6 Tárgyválasztás, listák és tárgyparancsok

**Tárgy bekérése (`get_item(cél, típus)`):**
```
if pack üres: "you aren't carrying anything"; return null
if 'a' ismétlés: az előző kiválasztott tárgy (ha elfogyott: "you ran out")
loop:
  "which object do you want to <cél>? (* for list): "     (terse: "<cél> what? (* for list): ")
  ESC → "" (törlés), return null, nem visz időt
  '*' → a csomag szűrt listája (inventory, a típusnak megfelelők), utána újra kérdez;
        ha nincs megfelelő: "you don't have anything appropriate", return null (nem visz időt)
  betű → ha nincs ilyen: "'<x>' is not a valid item", újra; különben return a tárgy
```
A típus csak a `*` listát szűri – bármelyik betű elfogadott; a hibás típust maga a parancs utasítja el.

**Listák 40 oszlopon** (csomaglista, `*` lista, `D`, győzelmi eladási lista):

> **C64-eltérés (E11):** a listák sorai 40 karakterre tördelődnek, a hosszú listák lapozódnak.

- Egy tétel: `<betű>) <név>` ([8.8](08_targyak.md#88-azonosítás-és-nevek)). Ha 40 karakternél hosszabb, szóhatáron törik, a folytatósor 3 szóközzel beljebb kezdődik.
- Megjelenítés az `inven` opció szerint:
  - *clear* – külön teljes képernyő, a 0. sortól. Ha a sorok nem férnek el a 0..23. sorban, a 24. sorban „--Press space to continue--”, és a lista új oldalon folytatódik.
  - *overwrite* – a térkép fölé, a képernyő jobb felső sarkába rajzolt keretben: szélessége a leghosszabb sor (legalább a „--Press space to continue--” hossza) + 1, magassága a sorok száma + 1. Ha így nem férne el a 0..24. sorban, *clear* módban jelenik meg. Bezáráskor a képernyő visszaáll ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).
  - *slow* – soronként az üzenetsávban, `--More--`-val.
- A lista végén „--Press space to continue--”; egyetlen sornál üzenetként jelenik meg. Üres csomag: „you are empty handed”.

**Egy tárgy (`I`):** üres csomag: „you aren't carrying anything”; egyetlen tárgy: azonnal „a) <név>”; különben „which item do you wish to inventory: ” → „<x>) <név>” vagy „'<x>' not in pack”.

**Fegyver kézbe (`w`):** ha a jelenlegi fegyver átkozott: „you can't.  It appears to be cursed” (időbe kerül). Páncélra: „you can't wield armor” (nincs idő). Már használt tárgyra: „That's already in use” (nincs idő). Siker: „you are now wielding <név> (<betű>)”.

**Páncél fel (`W`) / le (`T`):** [4.2.3](#423-tárgyak). Siker: „you are now wearing <név>” (a páncél azonosítódik), illetve „you used to be wearing <betű>) <név>”. Nem páncélra: „you can't wear that”.

**Gyűrű fel (`P`):** nem gyűrűre: „it would be difficult to wrap that around a finger” (időbe kerül); használt tárgyra: „That's already in use”; ha mindkét kéz szabad: „left hand or right hand? ” (`l`/`r`; ESC kilép, de a kör elmegy), különben a szabad kézre; két gyűrűnél: „you already have a ring on each hand”. Felhúzáskor: add strength → `chg_str(+mod)`; see invisible → CANSEE (a láthatatlan szörnyek kirajzolódnak); aggravate monster → aggravate ([6.4](06_szornyek.md#64-ébresztés)). Üzenet: „you are now wearing <név> (<betű>)”.

**Gyűrű le (`R`):** nincs gyűrű: „you aren't wearing any rings”; két gyűrűnél kérdezi a kezet. Átkozott: „you can't.  It appears to be cursed”. Levételkor: add strength → `chg_str(−mod)`; see invisible → a gyűrű-forrás megszűnik, a CANSEE csak akkor kapcsol ki, ha a see invisible bájital időzítője sem fut (J5). Üzenet: „was wearing <név>(<betű>)”.

**Elnevezés (`c`):** választható bármi, kivéve ételt („you can't call that anything”). Bájital/tekercs/gyűrű/pálca: a *típus* kap nevet (ha már azonosított: „that has already been identified”); minden más (fegyver, páncél, amulett): az adott tárgy saját címkét kap (ha nincs szabad címkehely: „you can't call that anything”, [1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)). Ha már volt név: „Was called "<név>"”; majd „what do you want to call it? ”. A beviteli mező a régi névvel, ha az nincs, az azonosítatlan névvel (szín, felirat, kő, anyag) van előtöltve, legfeljebb 20 karakterig.

**Felfedezések (`D`):** „for what type of object do you want a list? (* for all)” – `!` `?` `=` `/` vagy `*` (ESC kilép; más billentyűre: „Please type one of !?=/ (ESCAPE to quit)”). Az ismert vagy elnevezett típusok listája minden hívásnál **véletlen sorrendben**, a tárgynév-formátummal ([8.8](08_targyak.md#88-azonosítás-és-nevek)); `*`-nál a négy lista üres sorral elválasztva. Ha nincs semmi: „Haven't discovered anything about any potions” (scrolls / rings / sticks).

**Jel magyarázata (`/`):** „what do you want identified? ” → „'<jel>': <leírás>” (ESC-re az üzenetsáv törlődik, nincs válasz). Nagybetű → a szörny neve. Ismert jelek: `|` `-` wall of a room, `*` gold, `%` a staircase, `+` door, `.` room floor, `@` you, `#` passage, `^` trap, `!` potion, `?` scroll, `:` food, `)` weapon, ` ` solid rock, `]` armor, `,` the Amulet of Yendor, `=` ring, `/` wand or staff. Minden más (a `$` is): „unknown character”.

**Súgó (`?`):** „character you want help for (* for all): ” – egy billentyűre az ahhoz tartozó sor („<jel>	<leírás>”, ismeretlenre „unknown character '<x>'”); `*`-ra a teljes lista egy oszlopban, oldalanként.

> **C64-eltérés (E11):** a súgó egy oszlopban, oldalanként 23 sorral jelenik meg (0..22. sor), a 24. sorban „--Press space to continue--” (ESC: kilépés). Így mind a 48 sor látható (az eredeti 80×24-es kétoszlopos elrendezésben az utolsó három nem fért ki). A `!` sora (alshell) kimarad, az ESC sora `STOP` jellel szerepel.

Sorformátum: a jel (CTRL-es billentyűnél `^` + betű), utána a leírás tabulátorral kezdve (8-as tabpozíciók; irányos parancsnál a jel után `<dir>` áll a tab előtt; a két „run” sornak nincs jele). A `*` lista 48 sora:
```
?       prints help
/       identify object
h       left
j       down
k       up
l       right
y       up & left
u       up & right
b       down & left
n       down & right
        <SHIFT><dir>: run that way
        <CTRL><dir>: run till adjacent
f<dir>  fight till death or near death
t<dir>  throw something
m<dir>  move onto without picking up
z<dir>  zap a wand in a direction
^<dir>  identify trap type
s       search for trap/secret door
>       go down a staircase
<       go up a staircase
.       rest for a turn
,       pick something up
i       inventory
--- 2. oldal ---
I       inventory single item
q       quaff potion
r       read scroll
e       eat food
w       wield a weapon
W       wear armor
T       take armor off
P       put on ring
R       remove ring
d       drop object
c       call object
a       repeat last command
)       print current weapon
]       print current armor
=       print current rings
@       print current stats
D       recall what's been discovered
o       examine/set options
^R      redraw screen
^P      repeat last message
STOP    cancel command
S       save game
Q       quit
--- 3. oldal ---
F<dir>  fight till either of you dies
v       print version number
```
Az egyes irányok nagybetűs és CTRL-os változatainak („run left”, „run left until adjacent”, …) saját súgósora is van az egy billentyűs súgóban, de a `*` listában nem szerepelnek.

**Felvétel részletei:** lebegve semmi nem vehető fel (rálépéskor csendben nem történik semmi, a `,` parancsra „You can't.  You're floating off the ground!”). Arany: „you found <n> gold pieces”, és a szoba aranya 0 lesz (a kapzsi szörnyek ettől a játékos felé fordulnak). `m` irány: „you moved onto <név>”. `,` üres mezőn: „there is nothing here to pick up”.

---

## 4.3 Szintváltás és játék vége

```
// mindkét parancs * – nem visz el időt
descend():  // '>'
  if ISLEVIT: "you can't. You're floating off the ground!"; return
  if not on STAIRS: "I see no way down"; return
  level = min(level + 1, 255); new_level()    // E16
ascend():  // '<'
  if ISLEVIT: "you can't. You're floating off the ground!"; return
  if not on STAIRS: "I see no way up"; return
  if !has_amulet: "your way is magically blocked"; return
  level--; if level == 0: win()
  else: new_level(); "you feel a wrenching sensation in your gut"
```

> **C64-eltérés (E16):** a mélység 8 bites, legfeljebb 255. A 255. szinten a lépcső és a csapóajtó ([9.1](09_csapdak.md#91-rálépés)) is új 255. szintet generál. Az eredetiben a mélységnek nem volt felső korlátja.

**Halál:** a játékos elveszíti az aranya 10%-át (`gold -= gold/10`), pontszám = a maradék arany.

> **C64-eltérés (E11, E17):** a sírkő a 40 oszlopos képernyőre 9 oszloppal balra tolva jelenik meg, és nincs rajta évszám, mert a C64-nek nincs valós idejű órája.

- *Sírkő* (`tombstone` opció be), a képernyő 8. sorától; a szövegsorok a 19. oszlop körül középre igazítva (kezdőoszlop = `19 − (hossz + 1) / 2`):
```
              __________
             /          \
            /    REST    \
           /      IN      \
          /     PEACE      \
         /                  \
         |      <név>       |        ← 14. sor
         |    <arany> Au    |        ← 15. sor
         |   killed by a    |        ← 16. sor; „an” magánhangzónál, éhhalál/fagyásnál nincs névelő
         |    <halálok>     |        ← 17. sor
         |                  |        ← 18. sor (az eredetiben az évszám)
        *|     *  *  *      | *
________)/\\_//(\/(/\)/\//\/|_)_______
```
- *Sírkő nélkül:* a 22. sorban „Killed by a(n) <halálok> with <arany> gold” (éhhalál/fagyás esetén névelő nélkül); ha nem fér el, szóhatáron a 23. sorban folytatódik.
- Halálokok: szörny → a szörny neve; `arrow`, `bolt`, `dart` (névelővel), `hypothermia` (Ice monster), `starvation` (névelő nélkül).

**Kilépés** (`Q`): pontszám = arany, levonás nélkül.

**Győzelem:** a győzelmi képernyő ([függelék](fuggelek_uzenetkatalogus.md#mentés-kilépés-indítás-vége)), utána a csomag minden tárgya azonosítódik és eladásra kerül; pontszám = arany + a tárgyak értéke. Az eladási lista a „Worth  Item” fejléc után soronként `<betű>) <érték, 5 jegyre igazítva>  <név>`, végül „<n>  Gold Pieces”; tördelés és lapozás a 40 oszlopos listák szerint ([4.2.6](#426-tárgyválasztás-listák-és-tárgyparancsok)).

| tárgy | érték |
|---|---|
| étel | 2 × darab |
| fegyver | alapérték × (3 × (hplus + dplus) + darab) |
| páncél | alapérték + 100 × (9 − arm) + 10 × (alap védelem − arm) |
| tekercs, bájital | alapérték × darab; ha a típus nem volt ismert, a fele |
| gyűrű | alapérték (+ a kő értéke, [8.8](08_targyak.md#88-azonosítás-és-nevek)); protection / add strength / dexterity / increase damage: + 100 × mod, ha mod > 0, különben összesen 10; ha nem volt azonosítva, a fele |
| pálca/bot | alapérték + 20 × töltés; ha nem volt azonosítva, a fele |
| Amulett | 1000 |

A negatív érték 0-nak számít.

**Ranglista (top 10):**

> **C64-eltérés (E8):** a ranglista a `rogue.scr` fájlban van a `device` egységen. Felhasználóazonosító nincs, a „felhasználónként egy bejegyzés” szabály a **név** (pontos egyezés) szerint működik. Fájlzárolás nincs („The score file is very busy…” üzenet sincs).

Halál, kilépés és győzelem után „[Press return to continue]”, majd a ranglista beolvasása, frissítése, kiírása és visszaírása; utána szóközre vissza a címképernyőre.
```
Top Ten Rogueists:
   Score Name
 1  4130 bob: A total winner on level 26.
 2  1210 alice: killed on level 14 by a
         troll.
 3   302 bob: quit on level 3.
```
- Egy bejegyzés: pontszám, név, ok (`killed`, `quit`, `A total winner`, `killed with Amulet`), szint (győzelemnél a `max_level`, egyébként az aktuális), halálnál a halálok.
- Sorformátum: `%2d %5d %s: %s on level %d` (halálnál ` by <halálok>`) és `.`; ha nem fér el 40 karakterben, szóhatáron törik, a folytatósor 9 szóközzel beljebb kezdődik.
- Beszúrás: a pontszám szerint csökkenő sorrendben, ha nagyobb valamelyik meglévőnél. Névenként csak **egy** nem-győztes bejegyzés maradhat: ha az új jobb, a régi kiesik; ha az adott névnek már van jobb nem-győztes bejegyzése, az új nem kerül fel. Győzelmek korlátlanul.
- Az új bejegyzés kiemelve (inverz) jelenik meg.
- Ha a fájl nem létezik, üres ranglistával indul. Lemezhiba esetén „disk error: <kód>,<szöveg>”; a ranglista ilyenkor is kiíródik (a memóriában frissítve), csak nem mentődik.
- A címképernyő `S` menüpontja csak beolvassa és kiírja a ranglistát.
