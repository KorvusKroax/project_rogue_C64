# 3. Látótér és megjelenítés

A 3.1–3.2 a logikai SCREEN réteget írja le (mit lát és mire emlékszik a játékos); a 3.3–3.5 azt, hogyan kerül ez a C64 40×25-ös képernyőjére.

## 3.1 Felderítés
```
enter_room(pos):  // ajtón át belépéskor, teleportkor, új szinten
  player.room = a pos-t tartalmazó szoba; door_open(player.room) (ébresztés, 6.4)
  if !DARK(player.room) and !ISBLIND:
     a teljes szoba (falakkal) MAP → SCREEN, a szörnyek helyén a látható szörnyek (see_monst)

leave_room(pos):  // szobából ajtón át folyosóra lépéskor
  if MAZE(player.room): return
  floor_char = PHANTOM ? PASSAGE : (!DARK or ISBLIND) ? FLOOR : ' '  // amivé a szörnyek helye válik
  a szoba SCREEN-jén: sötét szobában a FLOOR jelek ' '-re törlődnek;
  a szörnyek jele helyére floor_char (ajtón DOOR) kerül – SEEMONST alatt inverzen maradnak
  player.room = a folyosórendszer (a B változatban: „folyosón”, 1.4.7); door_open(régi szoba)

look(wakeup):  // minden akció előtt
  ha elmozdult: erase_lamp(régi pozíció, régi szoba)   // új szinten és betöltés után a régi = a játékosé (J20)
  a 3×3 környezet minden mezője (a játékosé kivételével):
     a térképen kívüli és a ' ' mező kimarad
     ha sem ez, sem a játékos mezője nem DOOR, és az F_PASS-uk eltér: kimarad
        (folyosó és szoba közt csak ajtón át lát, folyosóról a szoba fala sem látszik)
     ha mindkettő F_PASS vagy DOOR, a mező átlós, és a két köztes mező közül
        egyik sem léphető (MAP-jel ' ', '|', '-'): kimarad (sarkon át nem lát; szobában nincs ilyen korlát)
     szörny: ha SEEMONST és a szörny ISINVIS: a mező teljesen kimarad (nem rajzol, nem ébreszt;
             CTRL-futásnál a futás megáll)
             különben wakeup esetén wake_monster; ha see_monst → a jele (hallucinálva véletlen betű),
             ha nem látszik → a MAP karaktere (trip_ch nélkül)
     egyébként a MAP karaktere, trip_ch-n át (hallucinálva véletlen tárgyjel, de csak ha az akció időt vitt)
  vakon csak a játékos mezőjét rajzolja, de a szomszédos szörnyek ébresztése vakon is lefut
  végül a játékos mezőjére PLAYER (futás jump mellett nem, 3.5)

erase_lamp(pos, room):  // csak sötét, nem fantom szobában, nem vak
  a pos körüli 3×3-ban a SCREEN FLOOR jelei ' '-re (a játékos mezője kivételével)

floor_at():  // a játékos mezőjének jele, amikor elhagyja (lépés, teleport): ez kerül a régi helyére
  ch = a MAP karaktere a játékos mezőjén
  if ch == FLOOR and PHANTOM(player.room): ch = PASSAGE
  return ch
```
- **Folyosón:** a folyosórendszer az A változatban PHANTOM és DARK szoba ([1.4.7](01_adatmodell_es_flagek.md#147-kijáratok-és-folyosók)); a B változatban a „folyosón” állapot ugyanígy számít a fenti vizsgálatokban (PHANTOM, DARK).
- **Lámpafény:** sötét szobában a játékos körüli 3×3 padlója mindig látszik, és lépéskor mögötte törlődik (`erase_lamp`).

  > **C64-javítás (J20):** új szinten és betöltés után a `look` előző pozíciója és szobája a játékosé, így az első `look` nem fut `erase_lamp`-ra. *Eredetileg* csak a játék indulásakor állítódtak be: új szinten az első `look` a régi szint pozíciója körül futtatta az `erase_lamp`-ot, a régi szoba sorszámával, amely már az új szint ugyanilyen sorszámú szobáját jelentette. Ha az sötét volt, a frissen kirajzolt világos kezdőszoba padlójába lyukakat törölhetett.

  > **C64-eltérés (E21):** nincs `seefloor` opció, a lámpafény mindig látszik. *Eredetileg* kikapcsolható volt (ekkor sötét szobában a padló sosem látszott), és 1200 baud vagy lassabb terminálon alapból ki volt kapcsolva, mert minden lépés kiírásokkal járt. C64-en a képernyőírás gyors, így az opciónak nincs szerepe.
- **Átlós rálátás:** a `look` átlós korlátja enyhébb, mint a mozgás sarokvágás-tilalma ([4.2.1](04_jatekciklus_es_parancsok.md#421-mozgás)): a rálátást csak az veszi el, ha mindkét köztes mező nem léphető, a lépést már egy is tiltja. Ami átlósan látszik, arra nem feltétlenül lehet átlósan lépni.
- **Ami a SCREEN-en marad (memória):** falak, ajtók, folyosók, lépcső, felfedezett csapdák **és a már látott tárgyak**. Sötét szobából távozva *kizárólag a FLOOR karakterek* törlődnek.
- **Tárgyak:** a látott tárgy emlékként a helyén marad akkor is, ha már nincs fény/rálátás. Az emlék nem frissül: ha közben egy szörny felveszi vagy elmozdítja, a képernyőn a régi jel marad, amíg a játékos újra nem látja a mezőt. Sötét szobában az új tárgy csak akkor jelenik meg, ha a 3×3-as környezetbe kerül; világos szobában belépéskor az összes látszik.
- **A játékos jele** a SCREEN-be is bekerül, mint bármely más jel, így görgetéskor ([3.5](#35-nézőablak-görgetés)) is a helyén van. Alatta nincs eltárolt emlék (`oldch`, mint a szörnyeknél): a játékos mindig látja a saját mezőjét, ezért távozáskor a `floor_at` a MAP-ból adja a visszarajzolandó jelet.
- **Szörnyek:** sosem maradnak emlékként. Csak akkor látszanak, ha *éppen* láthatók: világos szobában a játékossal azonos szobában, sötét szobában és folyosón csak a 3×3-ban (dist2 < 3), egyébként SEEMONST/CANSEE szabályai szerint. Ha nem látható, a helyén az alatta lévő emlékezett mező (`oldch`) jelenik meg.
- Rejtett (F_REAL nélküli) mezők a rejtett alakjukban jelennek meg, amíg fel nem fedezik őket.
- A láthatóság logikai fogalom: egy szörny akkor is „látható”, ha a mezője éppen kívül esik a nézőablakon ([3.5](#35-nézőablak-görgetés)). A nézőablak csak a kirajzolást vágja, a játékszabályokat nem befolyásolja.

## 3.2 Állapotok hatása a kijelzésre
- **ISBLIND:** a `look` csak a játékos saját mezőjét frissíti, szoba nem világosodik ki, szörny nem látszik (`see_monst` hamis). Visszanyeréskor (`sight`): ha a játékos szobában van, `enter_room`; „the veil of darkness lifts” (hallucinálva: „far out! Everything is all cosmic again”).
- **ISHALU** – két része van:
  - `look` közben (`trip_ch`): minden látott mező, ami nem padló, üres, folyosó, fal, ajtó vagy csapda – és nem a már látott lépcső –, véletlen tárgyjelként (`rnd_thing`, [6.2](06_szornyek.md#62-generálás)) rajzolódik; a látható szörnyek véletlen nagybetűként.
  - minden kör elején (`visuals` démon, BEFORE fázis; futás közben `jump` mellett kimarad): a látható tárgyak, a még nem látott lépcső és a látható szörnyek újrasorsolódnak (az álcázott Xeroc tárgyjelet kap); SEEMONST mellett a falon túli szörnyek is véletlen betűt kapnak. Ez a teljes szintre vonatkozik, nem csak a nézőablakra (a dobások száma így nem függ a nézőablak helyzetétől).
  - Lejáratkor (`come_down`): a látható tárgyak és szörnyek a valódi jelükkel rajzolódnak vissza; „Everything looks SO boring now.” Ha a játékos épp vak, csak a flag törlődik: nincs visszarajzolás és üzenet sem.
- **SEEMONST** (monster detection): bekapcsoláskor minden szörny a valódi típusjelével (hallucinálva véletlen betűvel) rajzolódik, a nem látottak inverzen; lejártakor a nem látott szörnyek helyére az `oldch` kerül. Ha egyetlen nem látott szörny sincs: „you have a strange feeling for a moment, then it passes” (hallucinálva „normal”).
- **CANSEE:** bekapcsoláskor (`invis_on`) a látható ISINVIS szörnyek kirajzolódnak (hallucinálva nem); kikapcsoláskor (`unsee`) helyükre az `oldch` kerül. A CANSEE két forrásból állhat fenn (bájital-időzítő, gyűrű), és csak akkor kapcsol ki, ha egyik sincs már (J5, [1.2.1](01_adatmodell_es_flagek.md#121-játékos)).
- **Mágia-észlelés:** külön képernyőn a szint és a szörnyek csomagjainak mágikus tárgyai `$` jellel; „You sense the presence of magic on this level”, utána `--More--`, billentyűre visszaáll; nem telik idő.
- **Étel-észlelés:** ugyanígy, az ételek `:` jellel ([8.3](08_targyak.md#83-tekercsek-r)).
- **Térképezés:** a MAP adott részei közvetlenül a SCREEN-re ([8.3](08_targyak.md#83-tekercsek-r)).
- A hallucinált üzenetváltozatok (`choose_str`): több üzenetnek van „trippy” párja (pl. „you feel less trippy now”, „the munchies are interfering with your motor capabilities”); ezeket az adott üzenetnél zárójelben jelöli a specifikáció, ahol eltérnek.

> **C64-eltérés (E11) – észlelés-képernyők:** a 80 oszlopos szint két félben jelenik meg. A térképterület (1..22. sor) törlődik, és először a bal fél (x = 0..39) jelei rajzolódnak ki a képernyő 0..39. oszlopába, az üzenettel és `--More--`-val; szóközre a jobb fél (x = 40..79) következik ugyanígy, a második félnél az üzenet helyén csak `--More--`. Ha az egyik félben nincs egyetlen jel sem, az a fél kimarad (az üzenet ilyenkor a megmaradt félnél jelenik meg). A végén a térkép a SCREEN-ből visszarajzolódik. A fejlesztői mód teljes térképe nem ilyen: az a nézőablakkal görgethető.

## 3.3 Státuszsor

> **C64-eltérés (E3):** az eredeti egysoros, kb. 75 karakteres státuszsor két 40 karakteres sorra oszlik; az értékek igazítás nélkül, egymás után, egy szóközzel elválasztva következnek. A `@` parancs (a státuszsor kiírása az üzenetsávba) elmarad, mert a státuszsor mindig látszik.

A 23. és 24. sorban:
```
23. sor: printf("Level:%d Gold:%d Hp:%d(%d) %s", level, gold, hpt, maxhp, hunger_word)
24. sor: printf("Str:%d(%d) Arm:%d Exp:%d/%d", str, str_max, 10 - player_armor(), lvl, exp)
// hunger_word: "", "Hungry", "Weak", "Faint"
// player_armor(): 5.5 – a viselt páncél (vagy az alap 10) a védelem-gyűrűkkel együtt (J11); páncél nélkül "Arm:0"
pl.  "Level:3 Gold:25 Hp:14(20) Hungry"
     "Str:16(16) Arm:5 Exp:2/38"
```
Mindkét sor a végéig törlődik. A legszélesebb reális értékekkel (255. szint, 6 jegyű arany, 3 jegyű HP, „Hungry”) a 23. sor pontosan 40 karakter; ha egy szélsőséges érték (pl. 7 jegyű arany) miatt hosszabb lenne, a vége levágódik. Minden kör végén kirajzolódik, összehasonlítás nélkül: a sor a régi fölé íródik, és csak a vége törlődik, így nem villog.

> **C64-javítás (J11):** a kijelzett Arm a védelem-gyűrűket is tartalmazza, vagyis azt az értéket mutatja, amellyel a harc számol ([5.5](05_jatekos.md#55-effektív-páncél-védekezéskor)). *Eredetileg* a státuszsor csak a viselt páncélt (vagy az alap 10-et) mutatta, a gyűrűk nélkül.

## 3.4 Üzenetsor

> **C64-eltérés (E2):** az üzenetsáv a 0. sortól `MESSAGE_ROW_COUNT` sor (C64-en 2), és ha a szöveg nem fér el az első sorban, a következőkre is átnyúlik. A további sorok ilyenkor ideiglenesen eltakarják a térkép legfelső sorait.

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `MESSAGE_ROW` | 0: az üzenetsáv első sora | 0 |
| `MESSAGE_ROW_COUNT` | 2: a sáv sorai; két 40 karakteres sor annyi, mint az eredeti 80 karakteres üzenetsor | 1 |
| `TEXT_MORE` | `--More--`: a sáv utolsó sorában mindig marad neki hely | `--More--` |

Fordítási feltételek: a sáv legalább egy sor, és nem érhet a státuszsorig. A sáv hossza 16 bites, így a sorainak számát csak a képernyő korlátozza.

- Az üzenetsáv `MESSAGE_ROW_COUNT`, egyenként 40 karakteres sor, **szótördeléssel**: a szöveg az utolsó olyan szóköznél törik, amely után a sor már nem férne el (40 karakternél hosszabb szó keményen törik). Két sor együtt egy 80 karakteres helynek felel meg, mint az eredeti üzenetsor.
- Minden üzenet nagybetűvel kezdődik (kivéve a tárgylista-sorokat, pl. „a) …”).
- **Automatikus pont:** az üzenet lezárásakor, ha az utolsó karaktere nem `.`, `!` vagy `?`, pont kerül a végére. Nem kap pontot a tárgylista-sor és a kérdés.
- Az egy körben keletkező üzenetek egy szóközzel összefűződnek. Ha az új üzenettel együtt a szöveg a tördelés után a `--More--`-val már nem férne el a sávban, az eddigi szöveg után `--More--` jelenik meg, és a játék szóközre vár; utána a sáv törlődik, és az új üzenet a 0. sor elején kezdődik.
- Ha egyetlen üzenet a `--More--`-val együtt nem fér el a sávban, az utolsó kiférő szó után jelenik meg a `--More--`, és szóközre a folytatás a törölt sáv elején kezdődik.
- **Hosszkorlát:** egy üzenet egy 128 bájtos pufferben készül el (`MESSAGE_TEXT_SIZE`), ebből a záró pontnak és a lezáró nullának 2 bájt jár, így legfeljebb 126 karakteres lehet; a hosszabb szöveg vége levágódik. A méret a leghosszabb valódi üzenet ismeretében csökkenthető.
- A következő billentyűleütéskor az üzenetsáv törlődik. `CTRL-P` az utolsó üzenetet ismétli; az „illegal command” és a kérdések nem számítanak bele (E20).

  > **C64-eltérés (E20):** a kérdések (pl. „which direction?”) nem számítanak utolsó üzenetnek, és az utolsó üzenet nem kerül a mentésbe. *Eredetileg* a kérdés is felülírta, így ESC után a `CTRL-P` a kérdést ismételte válaszmező nélkül; a mentésbe pedig bekerült, de betöltés után a „file name: <fájl>” üzenet azonnal felülírta, így sosem lehetett visszahozni. Az „illegal command” az eredetiben sem számított bele, így egy elütött billentyű nem takarja el a visszahívandó üzenetet.
- Amíg a sáv a térkép felső sorait foglalja, az ezekbe a sorokba rajzolás csak a SCREEN rétegbe kerül, a képernyőre nem; ha a sáv rövidebb lesz, vagy törlődik, a felszabaduló sorok a SCREEN-ből visszarajzolódnak.
- **Kérdések és bevitel** (`get_item`, `c`, mentési fájlnév, név): a kérdés szövege ugyanígy a sávba kerül, a beírt szöveg közvetlenül utána, tördelés nélkül folytatódik (ha a sor végére ér, a következő sorban). Ha a bevitel a sáv utolsó sorának végére érne, a beviteli mező nem nő tovább (a hosszkorlátokat lásd [1.4.4](01_adatmodell_es_flagek.md#144-szövegek)). DEL az utolsó karaktert törli, RETURN befejezi, ESC megszakít. A beírás helyét inverz szóköz jelöli. A kérdés a válasszal együtt látszik, amíg a következő üzenet vagy a következő leütés le nem váltja; az új üzenet nem fűződik hozzá. ESC-re a sáv azonnal törlődik.

## 3.5 Nézőablak (görgetés)

> **C64-eltérés (E1):** a 80 oszlopos logikai térképből a képernyőn egyszerre 40 oszlop látszik. Az ablak a játékost követi, holt zónával.

```
VIEW_WINDOW_WIDTH         = min(SCREEN_WIDTH, MAP_COLUMNS)    // a nézőablak szélessége: 40; keskenyebb térképnél a térképé, és nem görget
VIEW_WINDOW_X_MAX         = MAP_COLUMNS - VIEW_WINDOW_WIDTH   // 40
VIEW_WINDOW_EDGE_DISTANCE = 8                                 // ennyi oszlopnál közelebb a széléhez görget
view_window_x ∈ 0..VIEW_WINDOW_X_MAX                          // a képernyő 0. oszlopában látszó logikai oszlop

center_view_window():
  view_window_x = clamp(player.x - VIEW_WINDOW_WIDTH / 2, 0, VIEW_WINDOW_X_MAX)
  redraw_map()                                  // a térkép sorainak újrarajzolása a SCREEN-ből

follow_view_window():  // minden olyan lépés után, amely a játékos pozícióját megváltoztatta (mozgás, teleport)
  sx = player.x - view_window_x
  if sx < VIEW_WINDOW_EDGE_DISTANCE:                        // a játékos a holt zóna bal szélére kerül
     view_window_x = max(player.x - VIEW_WINDOW_EDGE_DISTANCE, 0)
  else if sx > VIEW_WINDOW_WIDTH - 1 - VIEW_WINDOW_EDGE_DISTANCE:      // a játékos a holt zóna jobb szélére kerül
     view_window_x = min(player.x - (VIEW_WINDOW_WIDTH - 1 - VIEW_WINDOW_EDGE_DISTANCE), VIEW_WINDOW_X_MAX)
  ha a view_window_x változott: a térkép újrarajzolása a következő show_map_changes-nél   // lent: Rajzolás
```
- `center_view_window` hívódik új szinten ([2](02_szintgeneralas.md)), és betöltés után; `follow_view_window` minden lépés után. Az ablak oszloponként követi a játékost, nem ugrik középre: egy lépésnél egy oszlopot mozdul, futás végén vagy teleportnál annyit, hogy a játékos a holt zóna szélére kerüljön. Görgetéskor a teljes térképterület újrarajzolódik (22 × 40 karakter), finom (pixeles) görgetés nem kell.
- **Futás `jump` mellett:** amíg a futás tart, nincs rajzolás; a futás végén `follow_view_window`, majd a térkép frissítése. `jump` nélkül minden lépés után `follow_view_window` fut.
- **Rajzolás:** minden SCREEN-írás (`draw_tile(x, y, ch)`) csak a SCREEN rétegbe kerül, és megjegyzi a változott sorokat. A videomemóriába a `show_map_changes` viszi, a játékos akciója után (a `look` után, a parancsbeolvasás előtt): egy képkocka elején a változott sorokat, görgetés után a teljes térképterületet, a nézőablak oszlopaival (`x − view_window_x`). Így egy lépés minden változása (a régi mező, a lámpafény, az új 3×3, a játékos jele) egyszerre jelenik meg, köztes kép nélkül; amíg semmi nem változik, nincs várakozás sem. Az üzenetsáv által foglalt felső sorok ([3.4](#34-üzenetsor)) kimaradnak, és felszabaduláskor a SCREEN-ből rajzolódnak vissza, így görgetés sem írja felül az üzenetet.
- **Animációk** (dobott tárgy, sugár, [7.4](07_harc.md#74-hajítás-t), [7.6](07_harc.md#76-elemi-sugár-lightning--fire--cold-pálca-sárkánytűz)): a nézőablakon kívül eső lépések nem rajzolódnak, és nincs utánuk várakozás sem.
- **Ideiglenes ablakok** (csomaglista *overwrite* módban, észlelés-képernyők, súgó, beállítások): csak a videomemóriába rajzolnak; bezáráskor a képernyő a SCREEN-ből és a státuszsorból visszaáll. Közben a játék nem halad.
