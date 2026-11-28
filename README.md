# project_rogue_C64

A *Rogue: Exploring the Dungeons of Doom* 5.4.4-es változatának újraírása **Commodore 64-re**.

<!-- TODO: cover kép, pl. ![cover](docs/img/cover.png) -->

## Build

### A játék fordítása és indítása

Szükséges: **Oscar64** (`C:\oscar64`) és **VICE** (`C:\GTK3VICE-3.10-win64`). Ha máshol vannak, a `build.bat` (`OSCAR64`), a `run.bat` és a `test.bat` (`VICE`) elején kell átírni az utat.

| parancs | mit csinál |
|---|---|
| `build.bat` | lefordítja a játékot: `build\rogue.prg` és `build\rogue.d64` (a lemezen a `rogue` és a szövegek fájlja, a `texts`) |
| `run.bat` | lefordítja, majd elindítja VICE-ban a lemezképről (autostart) |
| `build.bat dev` | a fejlesztői build: `build\rogue_dev.prg` és `build\rogue_dev.d64` |
| `build.bat dev 12345` | a fejlesztői build rögzített seeddel (itt 12345) |
| `run.bat dev`, `run.bat dev 12345` | ugyanez, és elindítja VICE-ban |
| `run.bat full`, `run.bat dev full` | bármelyik `run.bat` teljes képernyőn |
| `test.bat` | lefordítja az összes tesztprogramot, és a nem interaktívakat lefuttatja ([Tesztprogramok](#tesztprogramok)) |
| `test.bat core`, `test.bat core monsters` | csak a felsorolt tesztprogramokat; az interaktívat (`screens`) megnyitja VICE-ban |
| `test.bat skip level` | mindet, a felsoroltakat csak lefordítja |
| `test.bat help` | a használat és a tesztprogramok nevei |
| `build.bat test core` | csak lefordít egy tesztprogramot: `build\test_core.prg` és `build\test_core.d64` |

Mindegyik bárhonnan indítható (PowerShellben `.\build.bat`, `.\run.bat`, `.\test.bat`). A C64-en (vagy VICE-ban a lemezkép csatolása után) kézzel: `LOAD"ROGUE",8` vagy `LOAD"ROGUE",8,1`, majd `RUN`.

**A lemezkép tartalma:**

| fájl | tartalom |
|---|---|
| `rogue` | a játék, BASIC-indítósorral |
| `texts` | a játék szövegei és a tábláik; a játék induláskor tölti be a ROM-ok alatti RAM-ba, közben a 0. sorban „loading...” látszik ([10.2](docs/spec/10_platform.md#102-memória)) |

A `texts`-et a fordító ugyanabban a menetben írja ki, mint a játékot, a lemezképre és a `build` mappába is (`build\rogue.prgtexts.prg`). A lemeznek induláskor bent kell lennie: a `build\rogue.prg` önmagában, a lemezkép nélkül nem indul el, hanem „insert the game disk and press space” jelenik meg, és szóközre újrapróbálja. Később a képernyőmodul (`rogue.screens`) is ide kerül ([10.1](docs/spec/10_platform.md#101-célgép)).

A nem időkritikus játékfájlok méretre fordulnak (`#pragma optimize(size)`): `game`, `combat`, `message`, `monsters`, `items`, `item_names`, `pack`, `text_list`, `discoveries`, `equipment`, `input`, `status`, `player`, `timers`, `potions`, `main`, `platform/platform`, `platform/disk`. A körönként futó és a szintgeneráló fájlok (`chase`, `explore`, `move`, `map`, `view`, `level`, `rooms`, `corridors`, `rng`, `num`, `texts`, `platform/key`, `platform/screen`) `-O2`-vel fordulnak. Az új, ritkán futó kód (pl. a bájitalok, a tekercsek, a beállítások) is méretre fordul. Méretre fordított fájlban egyik függvény sem adhatja át a saját paraméterének címét, mert ott az Oscar64 hibás kódot fordít ([Oscar64-hibák](#oscar64-hibák-és-megkerülésük), 15.); ilyenkor a paramétert előbb helyi változóba kell másolni. A hely terve és a mért ár: [10.8](docs/spec/10_platform.md#108-erőforrás-keretek).

A `build.bat` a szabad hely mellett a dinamikus vermet is ellenőrzi, és figyelmeztet, ha abból kevesebb használható, mint a legnagyobb igény. A használható rész a `.map` STACK sora, 2 bájttal kevesebb, mert a verem ennyivel a sor vége előtt indul. A legnagyobb igény a játékban és a fejlesztői buildben 6 bájt (a listák visszahívásai, `show_text_list` és `draw_list_entries` 3–3 bájt), a tesztprogramokban 10 (a tesztfuttató, `run_test_table` még 4 bájt). A figyelmeztetés nem hiba, a fordítás elkészül. 2026-11-28-án a játékban 150, a fejlesztői buildben 36 bájt használható ([10.2](docs/spec/10_platform.md#102-memória)).

### Fejlesztői build

A `dev` paraméterrel a fordító a `DEVMODE` makróval fordít ([M00-04](https://github.com/KorvusKroax/project_rogue_C64/issues/4), [10.9](docs/spec/10_platform.md#109-fejlesztői-build-és-determinizmus)). A fejlesztői build a játék, és mellette csak ennyi; a kiadási buildbe ezekből semmi nem kerül:
- **`DEV_CHECK(feltétel, "üzenet")`** (`src/dev/dev.h`): hamis feltételnél kiírja az üzenetet és a forrásbeli helyet (fájl:sor), majd a program megáll; a képernyő és a memória marad, így a VICE monitorában megnézhető. A kiadási buildben nincs ilyen makró, ezért játékfájlban csak `#ifdef DEVMODE` alatt használható. Jelenleg csak a `map.c` térképírás-ellenőrzése használja, ideiglenesen, és mivel a fejlesztői buildben nincs rá hely, csak a tesztprogramokban (`#ifdef TEST_PROGRAM`).
- **Seed:** induláskor kiíródik; az `S` billentyűvel kézzel megadható, vagy a második paraméterrel (`build.bat dev 12345`) fordításkor rögzíthető, így egy hiba ugyanazzal a seeddel újra előidézhető. RETURN-re indul a játék.
- **Fejlesztői parancsok** játék közben, az F-billentyűkön (`src/dev/dev_commands.c`; a kiadási buildben az F-billentyűk nem adnak kódot), pl. új szint, ugrás mélységre, szörny és tárgy létrehozása, teljes térkép. A lista: [10.9](docs/spec/10_platform.md#109-fejlesztői-build-és-determinizmus). Egyik sem visz el időt.

A `src/dev/` fejlesztői fájljai (`dev.c`, `dev_commands.c`, `start_screen.c`, `checks.c`) ritkán futnak, ezért méretre fordulnak (`#pragma optimize(size)`); a teszteké nem.

Az öntesztek és a tesztképernyők nem a fejlesztői buildben vannak, hanem külön tesztprogramokban ([M05-05](https://github.com/KorvusKroax/project_rogue_C64/issues/55)), így a fejlesztői build memóriáját nem terhelik.

### Tesztprogramok

Minden tesztprogramnak saját belépési pontja és teszttáblája van (`src/dev/main_test_<név>.c`), és a `DEVMODE`, valamint a `TEST_PROGRAM` makróval fordul; az utóbbi alatt azok az ellenőrzések vannak, amelyekre a fejlesztői buildben nincs hely (most a térképírásé). A forráslista mindegyiknél a teljes játék és az összes tesztfájl (`build.bat`, `TEST_SOURCES`), de az Oscar64 csak azt fordítja bele, amit a program hív. Új tesztfájlt a `TEST_SOURCES`-ba kell felvenni; új tesztprogramhoz elég egy új `main_test_<név>.c`, a `test.bat` magától megtalálja. Ahol a tesztek üzenetet adó játékkódot futtatnak (pl. szörnytámadás), a program a sáv alatt kezdi a kiírást, és kikapcsolja a `--More--` várakozását (`set_message_more_wait(false)`, csak a `DEVMODE` fordításban létezik), mert a tesztben nincs, aki szóközt nyomjon. A billentyűre váró játékkódnak (pl. a `c` beviteli mezője) a teszt a `set_test_keys` leütéseit adja, ez is csak a `DEVMODE` fordításban létezik. A tesztprogramok a játékhoz hasonlóan induláskor betöltik a szövegeiket a lemezképükről (a szöveget nem használóké üres, ott a betöltés elmarad), ezért a `.d64`-ről kell indítani őket; a `test.bat` is így teszi.

| név | mit tesztel | szabad hely (2026-11-28) |
|---|---|---|
| `combat` | az erő harci táblái; a szörny neve az üzenetekben; a játékos és a szörny találati aránya 20 000 csapásból, rögzített maggal, a képletből számolthoz képest (±1%), a szörnyé a cselekvésképtelen játékos ellen is (+4), és a sebzés tartománya; a kézben tartott tárgy sebzése az erő bónuszával: üres kéz, bot és pálca (az anyaguk szerint), más tárgy, amelynek a rekordja nem számít bónusznak; a szörnyre lépő játékos támad, a szomszédos ébren lévő szörny támad; az `f`-harc szabálytalan átlónál nem indul, és véget ér a HP-határnál (kamikazénál nem), más szörny támadására, a célpont elmozdulásakor és halálakor; a szörny halála: kikerül a táblából, a játékos megkapja az XP-jét (16 777 215-nél telítődik); a szintlépés küszöbei és HP-ja; a különleges támadások: az Ice monster fagyasztása és a fagyhalál (a kikapcsolt Ice monster nem fagyaszt), a Vampire szívása és az aránya, a Wraith szintszívása (J2), a Rattlesnake marása (a mérgezés elleni mentődobás aránya két szinten, az erőcsökkenés), az erő határai és a legnagyobb erő, a Venus flytrap szorítása (a fogás és a mellé ütés sebzése, a kikapcsolt flytrap, a halála elengedi a játékost, a mellé ütés is ölhet); fogva tartva csak a flytrapra lehet lépni, a többi lépés időbe kerül, és véget vet a futásnak és az `f`-harcnak (J25); az álcázott xeroc megtámadása csak leleplezi, a saját támadása is leleplezi, vakon egyik sem; zavarodottan a lépések kb. 80%-a véletlen (a 3×3 mezői egyforma eséllyel), a helyben maradás nem visz időt, és véget vet a futásnak, a véletlen lépés szörnyre és kőzetbe nem lép, az álcázott xerocot megtámadja, és az `f`-harc a véletlen lépésnél véget ér, az ellépőnél is (J37); a játékos halála (utána a szörnyek nem lépnek, az f-harc véget ér) és az új játék kezdőértékei; a játékos védelme a viselt páncélé, páncél nélkül az alapérték, és ezt mutatja a státuszsor Arm értéke | 7 312 bájt |
| `core` | RNG, számok és szövegből szám, billentyűkódok, nézőablak-követés, üzenetsáv-tördelés, a 40 oszlopos lista sorai és az *overwrite* ablakba férése, a képernyő visszaállása az ideiglenes ablak után, a listasor (a betű nélküli is) nem fűződik össze más üzenettel, és a CTRL-P változatlanul hozza vissza, új játéknál pedig üres sávot hoz, a beviteli mezős kérdés csak akkor fűződik az előző üzenethez, ha a mező is elfér mögötte, térképflagek; a szövegek kiolvasása (a játék szövegeivel, a ROM-ok alól is; a `$01` és a megszakítás tiltása a hívás után ugyanaz), és a CIA2 NMI-je a kilapozott ROM melletti másolás közben visszatér | 20 864 bájt |
| `equipment` | a `w`: a `*` lista csak fegyver, páncélra és használt tárgyra elmarad, bármi más kézbe vehető, az átkozott tárgy a kézben marad; a `W`: a `*` lista csak páncél, nem páncélra üres kör nélkül, a páncél egy üres kör (a szörny egyet lép) után kerül fel, azonosítva, a második nem; a `T`: páncél nélkül nem visz időt, az átkozott rajta marad, egy üres kör után lekerül; a `d` a használatban lévő tárggyal: az átkozott marad, a kéz kiürül, egy darabnál is, a darabolás helye nélkül a köteg a kézben marad, a páncél üres körrel kerül le; a halál az üres körben véget vet a parancsnak; a kezdőfelszerelés 100 seeddel: betűk, sorrend, bónuszok, azonosítás, a nyilak száma, a viselt páncél és a kézben tartott buzogány; a `)` és a `]`: üres kéznél és páncél nélkül „nothing”, különben a betű és a név, utótag nélkül | 16 904 bájt |
| `explore` | átlós lépés, a szomszédos mezők láthatósága, futás (a vakon indított óvatos futás ajtó mellett sem áll meg, a látás visszatérte után sem; vakon nincs kanyarkövetés), keresés | 20 424 bájt |
| `items` | a tárgytípusok és -altípusok esélytáblái a 0..99 dobásokkal; az étel nélküli szintek után kényszerített étel (`no_food`); a tárgytábla betelése (utána nincs hely, és a véletlen tárgy nem dob) és felszabadítása, közben a foglaltság számlálója; a tárgylisták (beszúrás, törlés); a kezdőértékek és a kötegazonosítók; a nevek és az álnevek listái, az álnevek kiosztása, a tekercsfeliratok; a tárgynevek a spec példái szerint, az elnevezett típusok és a címkék `called` alakjával, és a kézben tartott tárgy és a viselt páncél utótagjával is, és a leghosszabb név 20 karakteres típusnévvel, címkével és utótaggal; a csomag helyei, betűi és rendezése; a felvétel: rálépve (a futás megáll) és `,`-vel, az `m` a padlón hagyja, az arany a pénzhez adódik, betelt csomagnál a tárgy a padlón marad, lebegve a rálépés (az `m` is) nem veszi fel, és nem ír üzenetet; a lerakás: bájitalkötegből egy darab új rekordként, a nyílköteg egészben, betelt tárgytáblánál a köteg darabolása elmarad; az amulett felvéve a játékosnál van, lerakva nincs; a névkészlet: betelés, felszabadítás, átnevezés, új játéknál üres, a fejlesztői mód foglaltsági üzenete; a címke a tárggyal felszabadul, a kötegbe olvadó tárgyét a címke nélküli köteg átveszi; a `c` listájából az étel és az amulett kimarad; a `c` menete leütésekkel: üres mezőre nincs név, átnevezés elírás javításával, ESC, törlés, RETURN után üres sáv, azonosított típus, fegyvercímke, a 20 karakteres korlát, étel és amulett, betelt névkészlet; a `D` listája: az ismert és az elnevezett típusok egyszer, a „semmi” sor, a `*` elválasztói, hívásonként új sorrend, az RN érintetlen | 1 944 bájt |
| `level` | 1000 véletlen szint rögzített magokkal, az 1., 10., 26. és 40. mélységen: a padlón lévő tárgyak (a mezőjükön a jelük, egy mezőn legfeljebb egy, a tárgytábla foglalt helyei éppen ezek), az arany (szobánként legfeljebb egy, a mennyisége a mélység szerinti tartományban), az amulett (a 26. mélységtől pontosan egy, alatta egy sem), a kincses szoba szörnyei, és az étel nélküli szintek számlálója szintenként eggyel nő, 255-nél megáll, étel után 0; utána mélységenként a sötét szobák és labirintusok aránya és a leglassabb generálás ideje; 10 maggal az amulettes szintek: a legmélyebb elért szint fölött nincs arany és tárgy, a legmélyebb elért szinten van, de újabb amulett nincs | 15 496 bájt |
| `monsters` | szörnytípusok, szörnytábla, generálás, a szörnyek mezőflagje, láthatósága, ébresztése (lebegő játékosra az aljas szörny nem ébred, a kapzsi igen) és üldözése; vakon semmi nem látszik: a szobába lépés és a körülnézés nem rajzol, de az ébresztés lefut, a lámpafény és kilépéskor a padló marad, a lépő szörny nem rajzolódik ki, és az emléke a képernyő jele; a látás visszatértekor szobában a szoba kirajzolódik, folyosón nem; a Medusa pillantása: egyszeri esély, a sikeres mentődobás után sem próbálkozik újra, világos szobában messziről, sötét szobában és folyosón csak a szomszédból néz, vakon, hallucinálva és kikapcsolva nem, a nem látott neve „something” | 7 968 bájt |
| `potions` | a bájitalok időzítői: körönként egyet lépnek, az üres körben is; az egy körben lejárók üzenetei a sorrendjükben (confusion, hallucination, see invisible, blindness, levitation, haste self, monster detection); vakon a hallucináció üzenet nélkül ér véget, akkor is, ha a vakság ugyanabban a körben jár le; a see invisible és a monster detection vége nem ad üzenetet; hallucinálva a lejáratok és a bájitalok hallucinált szövegváltozatai, a „normal feeling” is; a `q`: a `*` lista csak bájital, a nem bájital a csomagban marad, kötegből egy darab fogy, és a kéz akkor is kiürül, az utolsó darab rekordja felszabadul, és a kézben tartott utolsó darab után is üres a kéz; a bájitalok üzenetei a leírt sorrendben, futó vaksággal, hallucinációval és gyorsítással is, és amit a bájital megszüntet, annak az időzítője 0; a szörnyérzékelés üzenete csak akkor, ha nincs nem látott szörny; a magic detection csak tárgyas padlónál mutat valamit (a padló vagy egy szörny csomagjának mágikus tárgyát), ekkor azonosítódik, és a típus neve felszabadul, különben „strange feeling”; iváskor mely típusok azonosítódnak (a confusion hallucinálva nem), és az azonosított típus neve felszabadul; a többinél a „what do you want to call it?”: ESC-re és üres beírásra nincs név, a beírt név a típusé, az elnevezett típus nem kérdez, betelt névkészletnél a kérdés helyett az üzenet; ismételt iváskor az idő összeadódik, a monster detectioné nem változik; a poison és az extra healing a hallucináció időzítőjét is törli (J35); gyorsítva újra megitatva ájulás, utána nincs lassulás-üzenet; 50 seeddel: a poison, a gain strength és a restore strength hatása az erőre, a healing és az extra healing HP-ja és max. HP-ja a dobás szerint, a raise level, a 21. szinten csak az üzenet (J36) | 13 448 bájt |
| `screens` | interaktív tesztképernyők, menüből: karakterkészlet (`C`), billentyűzet (`K`), üzenetsáv és nézőablak (`M`), státuszsor (`S`); mindegyikből ESC-re vissza a menübe | 29 864 bájt |
| `turns` | 100 generált szinten (a `level` magjaival és mélységeivel) körök a játékos lépésével, előbb 5, aztán minden szörny üldöz: a lépéseik után is érvényesek a szörnymezők; utána mélységenként egy kör ideje; a szintek függetlenek, az időzítők (pl. a Medusa pillantásától a zavarodottság) minden szint elején nullázódnak | 10 896 bájt |

Összehasonlításul: a játék (`build.bat`) 5 144, a fejlesztői build (`build.bat dev`) 1 768 bájt szabad helyet hagy. A pontos érték minden fordítás végén kiíródik.

A `test.bat` a nem interaktív programokat VICE-ben futtatja, warp módban, a saját VICE-beállításaid nélkül (`-default`); a VICE ablaka magától bezárul. A tesztprogram a végén a $D7FF címre írja az eredményt (0 = minden teszt sikerült), és a VICE a `-debugcart` kapcsoló miatt ezzel a kilépési kóddal lép ki; valódi C64-en ez a cím egy nem használt SID-regiszter tükre, az írás hatástalan. Programonként kiíródik a szabad hely és az eredmény, a végén pedig a hibás programok listája (`[forditas]`: nem fordult le, `[teszt]`: valamelyik teszt nem sikerült, vagy a program nem ért véget, pl. egy `DEV_CHECK` megállította), ilyenkor a `test.bat` kilépési kódja 1. Ha a `build.bat` valamelyik programnál a dinamikus veremre figyelmeztetett, a végén azok is felsorolódnak, de ez nem hiba. A kilépéskori képernyő `build\test_<név>.png`, a VICE naplója `build\test_<név>.log`: hiba esetén ezekből látszik, melyik teszt nem sikerült.

Egy tesztprogram kézzel is elindítható, pl. a `build\test_level.d64` VICE-ba húzásával: az eredmény a képernyőn marad (`OK`, `1000/1000`, hibánál `FAIL AT <sorszám>`, alul `Done`). A hosszú tesztek **Alt+W**-vel gyorsíthatók (warp mód), és ESC-re leállnak; ilyenkor az addigi próbákból jön az eredmény.

### A két fordító összevetése

A `compare.bat` a szintgeneráló fájlokat (`rooms`, `corridors`, `map`, `rng`, `level`, `state`) változtatás nélkül, egy kis keretprogrammal (`src/dev/compare.c`) mindkét fordítóval, `-O2` és `-Os` mellett is lefordítja: `build\compare_<fordító>-<beállítás>.prg` (pl. `build\compare_llvm-mos-Os.prg`). Mindegyik mellé az üres keretprogram is lefordul (`src/dev/compare_empty.c`, a szintgenerálás helyett csonkokkal), és a két méret különbsége, a szintgenerálás kódja és állandó adata kiíródik. Ehhez az Oscar64 mellett az **llvm-mos** is kell (`C:\llvm-mos`); ha máshol van, a `compare.bat` elején (`LLVM_MOS`) kell átírni az utat. A PRG-k VICE-ben futnak (pl. a fájlt az ablakra húzva): 100 szintet generálnak, és mélységenként kiírják a generálás átlagos és leglassabb idejét képkockában, valamint a szintek összesített ellenőrzőösszegét, amelynek minden változatnál egyeznie kell. Az eredmények: [Eszközlánc](#eszközlánc), a háttér: [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54).

Projektszerkezet:
```
src/          a játék forrása
build/        a fordítás kimenete (nincs a repóban)
build.bat     fordítás
run.bat       fordítás + indítás VICE-ban
test.bat      a tesztprogramok fordítása és futtatása
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

**Ellenőrzés:** `test.bat screens`, majd a menüben `K` (`Keyboard matrix`). A `Last press` sorban a Shift+6 47-et, a Shift+7 61-et, az AltGr+W 124-et, a Shift+, (`?`) 63-at kell mutasson. SHIFT alatt ez a képernyő szándékosan nem olvas, ezért a szám a SHIFT felengedése után frissül.

### Eszközlánc

**Oscar64 1.32.273** (`C:\oscar64\bin\oscar64.exe`), C99. Döntés: [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1), megerősítve a valódi szintgenerálón: [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54). Az Oscar64 marad, mert az llvm-mos kódja nem kisebb, hanem 1,7–3,3-szor nagyobb, és az Oscar64-gyel a leglassabb szint is csak 44 képkocka (kb. 0,9 s) a 3 másodperces keretből ([Összevetés a szintgenerálón](#összevetés-a-szintgenerálón)).

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

- Mind a négy változat ugyanazt a 100 szintet adja (összesített ellenőrzőösszeg: `765E36E1`), és mind bőven belül van a 3 másodperces (150 képkockás) kereten ([10.8](docs/spec/10_platform.md#108-erőforrás-keretek)).
- Az Oscar64 mindkét szempontból jobb: már a `-Os` beállítása is kisebb és gyorsabb kódot ad, mint az llvm-mos bármelyike. Az llvm-mos kódja 1,7-szer (`-Os`; `-Oz`-vel 6 309 bájt) vagy 3,3-szor (`-O2`) akkora. Ha ez az arány a teljes játékra is áll, az akkor becsült 28–34 KB kód llvm-mos-szal 48–58 KB lenne, és nem férne el a 46 976 bájtos fő régióban (a 2026-11-27-i becslés szerint a kód ennél is nagyobb lesz, [10.8](docs/spec/10_platform.md#108-erőforrás-keretek)).
- Az Oscar64-en belül a `-Os` 10%-kal kisebb, de kb. 40%-kal lassabb kódot ad, mint az `-O2`.
- Az llvm-mos `-O2` a gyári zero page-kerettel (`-mlto-zp=110`) nem linkelhető (`relocation R_MOS_ADDR8 out of range`), 61 bájtig igen; a `compare.bat` 32-t ad meg.
- A fordítási idő a keretprogramon mindkettőnél 1 másodperc alatti.

**A váltás költsége.** A játéklogika változás nélkül fordul, Oscar64-specifikus rész csak a `src/platform/` alatt, a `build.bat`-ban és a `src/texts.c` elején (a szövegek szakaszának pragmái) van:

| Oscar64 | llvm-mos |
|---|---|
| `#pragma region(main, …)`, `#pragma heapsize(0)` | saját linkerszkript: RAM `$0880–$BFFF`, a verem a `$C000` alatt |
| `__asm raster_interrupt_handler { … }`, `__asm { sei }` | külön `.s` fájl vagy modulszintű `asm` (GNU szintaxis), `SEI()` / `CLI()` |
| `__interrupt`, `__forceinline` | `__attribute__((interrupt))`, `__attribute__((always_inline))` |
| `<c64/vic.h>`, `<c64/cia.h>`, `<c64/memmap.h>` (`vic.`, `cia1.`, `mmap_set`, `vic_setmode`) | `<c64.h>` (`VIC.`, `CIA1.`), a `$01`, `$D018`, `$DD00` közvetlen írása |
| `-d64=` | `c1541` a VICE-ből |
| `#pragma section(texts, …)`, `#pragma region(texts, …)`, `#pragma overlay(texts, 1)`: a szövegek a `$D000`-s régióban, külön fájlként a lemezen | a linkerszkriptben külön szakasz, a fájl kiírása külön lépésben |
| `<c64/kernalio.h>` (`krnio_`) | a KERNAL-hívások (`cbm_k_*`) |
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
Hol: `src/dev/tests.h`. Bejelentve: [#468](https://github.com/drmortalwombat/oscar64/issues/468); javítva a fő ágban, a következő kiadásban várható.

**2. Struct-tag inline asm-ben, és a globális változó asm után** – futáskor, csendben; VICE-ben ellenőrizve. Két külön hiba:
- A `változó.tag` forma asm-ben nem dokumentált, a fordító mégis szó nélkül elfogadja, és a tag eltolását elhagyja. A dokumentált forma a `Típus::tag` ([oscar64.md, Inline Assembler](https://github.com/drmortalwombat/oscar64/blob/main/oscar64.md#inline-assembler)), de ez csak C++ fájlban fordul; C-ben fordítási hiba. Bejelentve: [#471](https://github.com/drmortalwombat/oscar64/issues/471); a fő ágban a `változó.tag` forma már támogatott, a következő kiadásban várható.
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
Hol: `src/platform/platform.c`. Bejelentve: [#469](https://github.com/drmortalwombat/oscar64/issues/469); javítva a fő ágban, a következő kiadásban várható.

**4. Inline-olt hívás tömbindexként összetett értékadásban** – fordításkor, a fordító összeomlik (`-O2`, „Addressing Error”, kilépési kód 30). Csak akkor jön elő, ha a függvényt hívják és a fordító inline-olja, ezért egy még nem hívott modulban rejtve maradhat; a forrásban kell keresni.
```c
// rossz
game.rooms[choose_random_room()].flags |= ROOM_PHANTOM;

// jó: a hívás eredménye előbb helyi változóba
uint8_t room_index = choose_random_room();
game.rooms[room_index].flags |= ROOM_PHANTOM;
```
Hol: `src/rooms.c`. Bejelentve: [#470](https://github.com/drmortalwombat/oscar64/issues/470); javítva a fő ágban, a következő kiadásban várható.

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

**7. Feltételes kifejezésláncot visszaadó függvény inline-olva egy ciklusba** – futáskor, csendben; VICE-ben és a generált kódban ellenőrizve. `-O2`-n a ciklusba inline-olt függvény egyes ágai rossz értéket adnak: az elsőből kimaradt a `'0'` hozzáadása, a második a hívó egy másik változóját (a sor számát) adta vissza a maradék helyett; a harmadik ág helyes volt. A függvény saját, nem inline-olt kódja helyes. Az, hogy előjön-e, a fordító inline-olási döntésén múlik: a fejlesztői build tesztprogramokra bontása után jelent meg.
```c
// rossz: a 22×80-as ciklusba inline-olva az első két sor hibás
static uint8_t view_pattern(uint8_t x, uint8_t y)
{
    if (y == MAP_TOP_ROW) return '0' + x / 10;          // x / 10 lett
    if (y == MAP_TOP_ROW + 1) return '0' + x % 10;      // y lett
    return x % 10 == 0 ? '|' : 'a' + x / 10;
}

// jó
static __noinline uint8_t view_pattern(uint8_t x, uint8_t y) { … }
```
Hol: `src/dev/test_screens.c`.

**8. Előző hívás eredményét továbbadó „proxy” és egy inline-olt hívó** – futáskor, csendben, memóriakorrupcióval; VICE-ben és a generált kódban ellenőrizve. Ha egy függvényt (itt `prepare_monster_name`) több helyről is egy előző hívás eredményével hívnak meg, a fordító `@proxy` változatot készít, amely a paramétert (P4) az ACCU-ból tölti be. Egy inline-olt hívóban (`attack_monster` a `move_player`-ben) ezután a paraméter feltöltése a proxyra marad, de az inline-olt kód már előtte használja a P4-et: a `start_monster_running` a szörny helyett egy véletlen címre írt. A játék buildjében nem jött elő, csak a `combat` tesztprogramban, mert ott a teszt adta a második ilyen hívóhelyet; hogy előjön-e, a teljes program hívásain múlik.
```c
// rossz: két hívóhely adja tovább egy hívás eredményét, így proxy keletkezik
struct Monster *monster = put_combat_scene();
prepare_monster_name(monster);
… prepare_monster_name(put_combat_monster(…)) …

// jó: a rekord címe a táblából
put_combat_scene();
prepare_monster_name(&game.monsters[0]);
```
Hol: `src/dev/test_combat.c`; ugyanez a `monsters` tesztprogramban is előjött (`src/dev/test_chase.c`, `src/dev/test_explore.c`): a beágyazott `start_monster_running` a `choose_monster_destination@proxy` hívása előtt írta a szörny flagjeit, ezért a szörny nem indult el. A generált kódban (`build/<név>.asm`) a `@proxy` nevű függvényeket és az előttük használt paraméterregisztereket érdemes megnézni, ha egy érték lehetetlennek látszik.

**9. Az optimalizáló nem jut a végére egy függvénynek** – fordításkor, figyelmeztetéssel: `warning 2007: Optimizer locked in infinite loop 'is_something_interesting_beside'`, előtte lépésenként 11 `Oops 7`, `Oops 8`, `Oops 9` sor. Az optimalizáló lépései addig ismétlődnek, amíg változtatnak a kódon: az `Oops N` azt jelzi, hogy az N. lépés 190-nél többször ismétlődött, a 200. körnél pedig a fordító abbahagyja ezt a lépést, figyelmeztet, és a következővel folytatja. A félbehagyott lépés csak az optimalizálást hagyja abba; a figyelmeztetéses buildben is minden teszt átment, köztük az óvatos futásé, amely ezt a függvényt ellenőrzi. Csak a `combat` tesztprogramban jött elő, amikor egy teszt a `take_run_step`-et hívta, és vele az `is_something_interesting_beside` is a programba került; a játékban és a fejlesztői buildben, ahol a függvény a `take_run_step`-be olvad, nem. Hogy előjön-e, a teljes program hívásain múlik.
```c
// rossz: a combat tesztprogramban
start_running(-1, 0, false);
take_run_step();

// jó: a futás lépése a move_player, a teszt így is ugyanazt ellenőrzi
start_running(-1, 0, false);
move_player(-1, 0);
```
Hol: `src/dev/test_combat.c`. Ismert: [#415](https://github.com/drmortalwombat/oscar64/issues/415) ugyanezt a jelenséget írja le; javítva a fő ágban (db75ffa: egy lépés egy `LDY`/`STY` pár miatt oda-vissza mozgatta ugyanazt a kódot), a következő kiadásban várható. Hogy nálunk ugyanez az ok, az csak a javított fordítóval derül ki.

**10. Közvetlen hívás 8 bites eredménye rossz helyről olvasva** – futáskor, csendben, memóriakorrupcióval; VICE-ben (pillanatképből kiolvasva) és a generált kódban ellenőrizve. Az Oscar64 egyes függvényeknél szándékosan az A regiszterben adja vissza a 8 bites eredményt, és ilyenkor a hívó is onnan veszi (pl. `find_monster_at` után `TAX`). A `create_gold` a fejlesztői buildben a sikeres ágon az A-ban tért vissza (`LDA T2; RTS`), a két hívója viszont az ACCU-ból olvasott (`JSR create_gold; LDA ACCU+0`), ahol éppen a `get_item` által visszaadott rekordcím alsó bájtja volt. Ez az „index” (pl. 203) a `put_item_on_floor`-ba került, amely a tárgytáblán túl, a térképre írt: a térképen idegen jelek jelentek meg (`;`, üres és inverz jelek), a padlólista és a foglaltság elromlott. A kiadási játékban és a tesztprogramokban a függvény a hívóba olvadt, és ott helyes kód lett belőle, ezért az öntesztek nem vették észre. Egyetlen `return`-nel ugyanígy fordult; a szétbontás segített.
```c
// rossz: a fejlesztői buildben az eredményt a hívó rossz helyről olvassa
uint8_t create_gold(void)
{
    uint8_t index = create_item(ITEM_TYPE_GOLD, 0);
    if (index == NO_ITEM) return NO_ITEM;
    uint16_t amount = rnd(…) + GOLD_AMOUNT_MIN;
    get_item(index)->properties.gold_amount = amount;
    return index;
}

// jó: a 8 bites eredményt a create_item adja, a mennyiséget egy void függvény írja
uint8_t gold_index = create_item(ITEM_TYPE_GOLD, 0);
if (gold_index != NO_ITEM) {
    set_random_gold_amount(gold_index);
    put_item_on_floor(gold_index, x, y);
}
```
Hol: `src/items.c`, `src/level.c`, `src/dev/dev_commands.c`. Hogy előjön-e, a fordító beillesztési döntésén múlik. Ellenőrzés: a `build/<név>.asm` fájlokban azok a 8 bites függvények, amelyek `LDA …` / `RTS` párral térnek vissza, és a hívójuk a `JSR` után az `ACCU + 0`-t olvassa.

**11. Mutatón át módosított változóról a hívás előtt készült másolat** – futáskor, csendben; VICE-ben és a generált kódban ellenőrizve (`-O2`). Ha egy helyi változóról másolat készül, utána a változó címét egy függvény kapja, és az a mutatón át módosítja, a fordító a másolat helyett a változó hívás utáni értékét használja: a kettőt egynek veszi, és nem számol azzal, hogy a hívás megváltoztatta. A listában így a sor kirajzolása a szöveg vége utáni szemetet kapta.
```c
// rossz: a row helyett a továbbléptetett text megy át
const char *row = text;
uint8_t length = take_entry_row(&text, is_continuation);   // a *text-et a következő sorra lépteti
draw_list_row(left, text_column, y, row, length);

// jó: a függvény nem írja a hívó változóját, a léptetés a hívóban történik
uint8_t length = calculate_entry_row_length(text, is_continuation);
draw_list_row(left, text_column, y, text, length);
text = skip_break_space(text + length);
```
Hol: `src/text_list.c`. Nincs bejelentve.

**12. Állandóval közvetlenül és függvénymutatón át is hívott függvény paramétere** – futáskor, csendben, végtelen ciklussal; VICE-ben (pillanatképből kiolvasva) és a generált kódban ellenőrizve. A `get_pack_list_entry(index)`-et a `show_pack_list` egyszer közvetlenül hívta 0-val (üres-e a lista), utána a címét a `show_text_list` kapta, amely függvénymutatón át 0, 1, 2… indexszel hívja. A fordító a közvetlen hívás állandóját beépítette a függvénybe: a lefordított kód a paramétert nem olvasta, és az `index--` ág is kimaradt. Így minden indexre az első sort adta, a lista hossza sosem ért véget, és az `i` két tárgytól lefagyott. A játékban és a fejlesztői buildben fordult így, a tesztprogramokban helyesen, ezért az öntesztek nem vették észre. A csak mutatón át hívott `get_discovery_entry` helyes.
```c
// rossz: a mutatón át érkező index elvész
if (get_pack_list_entry(0) == NULL) { … }
show_text_list(get_pack_list_entry);

// jó: a függvényt csak a mutató hívja
if (!has_pack_list_entry()) { … }
show_text_list(get_pack_list_entry);
```
Hol: `src/pack.c`. Nincs bejelentve.

**13. Üres overlay-szakasz fájlja** – fordításkor, csendben. Ha egy overlayként kiírt szakaszba (`#pragma overlay`) semmi nem kerül, a fordító akkor is ír egy fájlt a lemezképre, de az 255 bájt, nullákkal és egy záró bájttal, betöltési cím nélkül. Azokban a tesztprogramokban jön elő, amelyek egyetlen szöveget sem használnak (pl. `explore`): ott a `texts` szakasz üres, és a betöltés ezt a fájlt hibásnak találta, ezért a program az „insert the game disk and press space”-nél állt meg.
```c
// jó: üres szakasznál a betöltés el sem indul
uint16_t remaining = (char *)&texts_section_end - (char *)&texts_section_start;
if (remaining == 0) return true;
```
Hol: `src/platform/platform.c` (`load_texts`). Nincs bejelentve.

**14. A 8 bites eredményt a függvény kilépő kódja felülírja** – futáskor, csendben; VICE-ben és a generált kódban ellenőrizve. Az `execute_wear_command` (bool) egy korábbi ágon `return false`-szal lépett ki, a végén pedig a beillesztett `wear_armor` eredményét adta vissza (`return wear_armor(index);`). A fordító az eredményt az A regiszterbe töltötte (`LDA #$01`), de a kilépő kód a mentett regisztereket az A-n át állította vissza (`LDA $bf23,x`), és az ACCU-ba semmit nem írt. A hívó az ACCU-ból olvasott, ahol a bekért tárgy indexe maradt, így a játék az idő nélküli választ is időt vivőnek vette: a `W` ismétlésére a „you are already wearing some” után a szomszédos kígyó támadott. Az `execute_drop_command` ugyanígy fordult. Az ugyanilyen alakú `execute_wield_command` helyes lett (ott a kilépő kód `STA ACCU + 0`-val kezdődik), tehát a fordító döntésén múlik. A tesztprogramok a parancsok belső függvényeit közvetlenül hívják, ezért az öntesztek nem vették észre.
```c
// rossz: a wear_armor eredménye elvész
uint8_t index = get_command_item(…);
if (index == NO_ITEM) return false;
return wear_armor(index);

// jó: egy kifejezésben
uint8_t index = get_command_item(…);
return index != NO_ITEM && wear_armor(index);
```
Hol: `src/game.c` (`execute_drop_command`, `execute_wear_command`). Ellenőrzés: a `build/<név>.asm` fájlokban a 8 bites függvények azon kilépő blokkjai, amelyek a `@stack`-ből állítják vissza a regisztereket, és nem `STA ACCU + 0`-val kezdődnek. Ha egy ág közvetlenül előttük az A-ba tölti az eredményt, az elvész. Nincs bejelentve.

**15. A címével átadott, közben megváltozott paraméter a hívóban elavult értékkel** – futáskor, csendben; csak méretre fordítva (`-Os` vagy `#pragma optimize(size)`); VICE-ben és a generált kódban ellenőrizve, a forrás `-Os`-sel fordított másolatán. Ha egy függvény a saját paraméterének címét adja át, és a hívott függvény ezen át megváltoztatja, a hívó a következő hívás argumentumait a zero page paraméterregisztereiből számolja tovább (pl. `DEC P8`), mintha azokban még az előző hívás argumentumai lennének. A `test_passage_turn` két egymás utáni `is_test_turn` hívása így a második hívásnak (0, −1) helyett (−1, 0)-t adott át, egy utána következő `put_int(2, 0)` pedig −16777214-et írt ki. `-O2`-n a segédfüggvény a hívóba olvad, a paraméterei a keretben vannak, és helyes.
```c
// rossz méretre fordítva: a step_x és a step_y regiszterét a find_passage_turn átírja,
// a hívó a következő hívás argumentumait mégis a régi értékből számolja
static bool is_test_turn(int8_t step_x, int8_t step_y, int8_t expected_x, int8_t expected_y)
{
    if (!find_passage_turn(BASE_X + 2, BASE_Y + 2, &step_x, &step_y)) return false;
    return step_x == expected_x && step_y == expected_y;
}

// jó: a paraméter helyi változóba másolva, annak a címe megy át
int8_t turn_x = step_x;
int8_t turn_y = step_y;
if (!find_passage_turn(BASE_X + 2, BASE_Y + 2, &turn_x, &turn_y)) return false;
```
Hol: csak a forrás `-Os`-sel fordított másolatán jött elő (`src/dev/test_explore.c`); a build `-O2`-vel fordít. Méretre csak a nem időkritikus játékfájlok ([A játék fordítása és indítása](#a-játék-fordítása-és-indítása)) és a fejlesztői fájlok ([Fejlesztői build](#fejlesztői-build)) fordulnak. Ezekben egyik függvény sem adja át a saját paraméterének címét; ha ott ilyen hívás kell, a paramétert előbb helyi változóba kell másolni. Nincs bejelentve.

**Tárgytalan platformcsapdák.** Ezek nem fordítóhibák voltak, és a BASIC-be visszatérés elhagyásával megszűntek ([10.1](docs/spec/10_platform.md#101-célgép)):
```c
// a BASIC ROM visszalapozása után a $B000 fölötti C-verem helyén ROM látszott
platform_exit();
put_text("bye");                    // összeomlott

// a fordító zero page-e felülírja a BASIC szövegverem-mutatóit ($16–$18)
return 0;                           // a BASIC-ben néhány parancs után ?FORMULA TOO COMPLEX
```

## Dokumentáció

- [docs/spec/](docs/spec/) – a C64-es változat specifikációja: a Rogue 5.4.4 minden szabálya, képlete, táblája és üzenete, a C64 képernyőjére, billentyűzetére, memóriájára és lemezkezelésére átdolgozva. A platform miatti eltérések (E1–E24) és az eredeti hibáinak javításai (J1–J38) jegyzéke: [0.3](docs/spec/00_attekintes.md#03-eltérések-és-javítások-jegyzéke); a hardverközeli részletek: [10. Platform](docs/spec/10_platform.md).
- [Mérföldkövek](https://github.com/KorvusKroax/project_rogue_C64/milestones) és [issue-k](https://github.com/KorvusKroax/project_rogue_C64/issues) – a megvalósítás mérföldkövenként (M0–M12), issue-nként tartalommal és „Kész, ha” feltételekkel (áttekintés lent).

## Mérföldkövek és issue-k

Az issue-k GitHubon vannak, mindegyikben: „Függ” (előfeltételek), „Spec” (a vonatkozó fejezetek), „Tartalom” és „Kész, ha” (elfogadási feltételek). Az M4-től kezdve minden mérföldkő végén a program elindul, és az addig kész rész kipróbálható. A belső ellenőrzések a fejlesztői buildben (M00-04), az öntesztek a tesztprogramokban futnak (M05-05).

| mérföldkő | issue-k |
|---|---|
| **M0 – Eszközlánc és platformalap** | [M00-01](https://github.com/KorvusKroax/project_rogue_C64/issues/1) eszközlánc kiválasztása · [M00-02](https://github.com/KorvusKroax/project_rogue_C64/issues/2) build és indítás · [M00-03](https://github.com/KorvusKroax/project_rogue_C64/issues/3) memóriatérkép, platformréteg · [M00-04](https://github.com/KorvusKroax/project_rogue_C64/issues/4) fejlesztői build |
| **M1 – Alapok** | [M01-01](https://github.com/KorvusKroax/project_rogue_C64/issues/5) RNG, aritmetika |
| **M2 – Képernyő és bevitel** | [M02-01](https://github.com/KorvusKroax/project_rogue_C64/issues/9) karakterkészlet · [M02-02](https://github.com/KorvusKroax/project_rogue_C64/issues/10) billentyűzet · [M02-03](https://github.com/KorvusKroax/project_rogue_C64/issues/11) nézőablak · [M02-04](https://github.com/KorvusKroax/project_rogue_C64/issues/12) üzenetsáv · [M02-05](https://github.com/KorvusKroax/project_rogue_C64/issues/13) státuszsor |
| **M3 – Szintgenerálás** | [M03-01](https://github.com/KorvusKroax/project_rogue_C64/issues/14) szobák, labirintus · [M03-02](https://github.com/KorvusKroax/project_rogue_C64/issues/15) folyosók · [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16) elhelyezés · [M03-04](https://github.com/KorvusKroax/project_rogue_C64/issues/17) fejlesztői mód · [M03-05](https://github.com/KorvusKroax/project_rogue_C64/issues/54) eszközlánc-ellenőrzés |
| **M4 – Mozgás, látótér, futás** | [M04-01](https://github.com/KorvusKroax/project_rogue_C64/issues/18) főciklus · [M04-02](https://github.com/KorvusKroax/project_rogue_C64/issues/19) mozgás, látótér · [M04-03](https://github.com/KorvusKroax/project_rogue_C64/issues/20) futás · [M04-04](https://github.com/KorvusKroax/project_rogue_C64/issues/21) keresés, lépcső, ismétlés |
| **M5 – Szörnyek és harc** | [M05-01](https://github.com/KorvusKroax/project_rogue_C64/issues/22) generálás · [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23) ébresztés, mozgás · [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) közelharc · [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) speciális támadások · [M05-05](https://github.com/KorvusKroax/project_rogue_C64/issues/55) tesztprogramok |
| **M6 – Tárgyak és csomag** | [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26) generálás · [M06-02](https://github.com/KorvusKroax/project_rogue_C64/issues/27) fegyverek, nevek · [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) csomag, listák · [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29) felszerelés · [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) szörnyek és tárgyak · [M06-06](https://github.com/KorvusKroax/project_rogue_C64/issues/56) szövegek a ROM-ok alatt · [M06-07](https://github.com/KorvusKroax/project_rogue_C64/issues/57) hely a fejlesztői buildben |
| **M7 – Bájitalok és tekercsek** | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) bájitalok, időzítők · [M07-02](https://github.com/KorvusKroax/project_rogue_C64/issues/32) kijelzési hatások · [M07-03](https://github.com/KorvusKroax/project_rogue_C64/issues/33) tekercsek · [M07-04](https://github.com/KorvusKroax/project_rogue_C64/issues/53) szövegkatalógus generálása · [M07-05](https://github.com/KorvusKroax/project_rogue_C64/issues/58) hely a játékban és a fejlesztői buildben · [M07-06](https://github.com/KorvusKroax/project_rogue_C64/issues/59) szövegtömörítés, kód a KERNAL alatt |
| **M8 – Gyűrűk, pálcák, hajítás** | [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) gyűrűk · [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35) hajítás · [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) pálcák, sugarak |
| **M9 – Csapdák, éhség, vándorlók** | [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) csapdák · [M09-02](https://github.com/KorvusKroax/project_rogue_C64/issues/38) éhség · [M09-03](https://github.com/KorvusKroax/project_rogue_C64/issues/39) regeneráció, vándorlók |
| **M10 – Játék vége, lemezkezelés, címképernyő** | [M10-01](https://github.com/KorvusKroax/project_rogue_C64/issues/40) játék vége · [M10-02](https://github.com/KorvusKroax/project_rogue_C64/issues/41) lemezkezelő réteg · [M10-03](https://github.com/KorvusKroax/project_rogue_C64/issues/42) ranglista · [M10-04](https://github.com/KorvusKroax/project_rogue_C64/issues/43) mentés, betöltés · [M10-05](https://github.com/KorvusKroax/project_rogue_C64/issues/44) címképernyő, beállítások |
| **M11 – Véglegesítés** | [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) szövegek, képernyők · [M11-02](https://github.com/KorvusKroax/project_rogue_C64/issues/46) robot, invariánsok · [M11-03](https://github.com/KorvusKroax/project_rogue_C64/issues/47) végigjátszás · [M11-04](https://github.com/KorvusKroax/project_rogue_C64/issues/48) erőforrások, kiadás |
| **M12 – Opcionális kiegészítések** | [M12-01](https://github.com/KorvusKroax/project_rogue_C64/issues/49) színek · [M12-02](https://github.com/KorvusKroax/project_rogue_C64/issues/50) hangok · [M12-03](https://github.com/KorvusKroax/project_rogue_C64/issues/51) botkormány · [M12-04](https://github.com/KorvusKroax/project_rogue_C64/issues/52) gyorsabb lemez |

## A javítások (J1–J38) helye
| javítás | issue |
|---|---|
| J1 a játékos ISRUN-ja | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25), [M09-02](https://github.com/KorvusKroax/project_rogue_C64/issues/38) |
| J2 Wraith | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) |
| J3 `find_dest` | [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J4 flytrap | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25), [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J5 see invisible | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31), [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) |
| J6 sárkányláng | [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J7 kincses szoba | [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26) |
| J8 „vanishes” üzenet | [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35) |
| J9 magic missile | [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J10 csapda és páncél | [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
| J11 státuszsor Arm | [M02-05](https://github.com/KorvusKroax/project_rogue_C64/issues/13), [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29), [M08-01](https://github.com/KorvusKroax/project_rogue_C64/issues/34) |
| J12 Xeroc és repülés | [M08-02](https://github.com/KorvusKroax/project_rogue_C64/issues/35), [M08-03](https://github.com/KorvusKroax/project_rogue_C64/issues/36) |
| J14 haste self | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
| J15 `f` átlósan | [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) |
| J16 lépcső csapdán | [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16), [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
| J17 elírások | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25), [M06-02](https://github.com/KorvusKroax/project_rogue_C64/issues/27), [M11-01](https://github.com/KorvusKroax/project_rogue_C64/issues/45) |
| J18 labirintus kijárata | [M03-02](https://github.com/KorvusKroax/project_rogue_C64/issues/15) |
| J19 a szint berendezése mindig véget ér | [M03-03](https://github.com/KorvusKroax/project_rogue_C64/issues/16), [M05-01](https://github.com/KorvusKroax/project_rogue_C64/issues/22), [M06-01](https://github.com/KorvusKroax/project_rogue_C64/issues/26), [M09-01](https://github.com/KorvusKroax/project_rogue_C64/issues/37) |
| J20 a lámpafény törlése új szinten | [M04-02](https://github.com/KorvusKroax/project_rogue_C64/issues/19), [M10-04](https://github.com/KorvusKroax/project_rogue_C64/issues/43) |
| J21 a kanyarkövetés az indulásnál | [M04-03](https://github.com/KorvusKroax/project_rogue_C64/issues/20) |
| J22 véletlen lépés és Xeroc | [M05-02](https://github.com/KorvusKroax/project_rogue_C64/issues/23) |
| J23 az `f` célpontjának jelölése | [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) |
| J24 a kamikaze csak egy harcra szól | [M05-03](https://github.com/KorvusKroax/project_rogue_C64/issues/24) |
| J25 fogva tartás és futás | [M05-04](https://github.com/KorvusKroax/project_rogue_C64/issues/25) |
| J26 az amulett címkéje | [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) |
| J27 ESC és a számprefixes ismétlés | [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) |
| J28 felvétel üres csomagba | [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) |
| J29 az amulett a csomagból adódik | [M06-03](https://github.com/KorvusKroax/project_rogue_C64/issues/28) |
| J30 az `a` tárgya, amíg a csomagban van | [M06-04](https://github.com/KorvusKroax/project_rogue_C64/issues/29) |
| J31 a nimfa és a köteg | [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J32 a szörny tárgyfelvétele látható mezőn | [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J33 a tárgy felé tartó szörny útjában a játékos | [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J34 a `W` üres körében ellopott páncél | [M06-05](https://github.com/KorvusKroax/project_rogue_C64/issues/30) |
| J35 a hallucináció időzítője | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
| J36 raise level a legmagasabb szinten | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
| J37 a zavarodott lépés és az `f`-harc | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
| J38 a zavarodott irány az egész parancsra szól | [M07-01](https://github.com/KorvusKroax/project_rogue_C64/issues/31) |
