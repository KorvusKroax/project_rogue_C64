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
  player.room = a folyosórendszer; door_open(régi szoba)

look(wakeup):  // minden akció előtt
  ha elmozdult: erase_lamp(régi pozíció, régi szoba)
  a 3×3 környezet minden mezője (a játékosé kivételével):
     ' ' mező kimarad; folyosó és szoba közt csak ajtón át lát (F_PASS-egyezés);
     sarkon át átlósan nem lát
     szörny: ha SEEMONST és a szörny ISINVIS: a mező teljesen kimarad (nem rajzol, nem ébreszt;
             CTRL-futásnál a futás megáll)
             különben wakeup esetén wake_monster; ha see_monst → a jele (hallucinálva véletlen betű),
             ha nem látszik → a MAP karaktere (trip_ch nélkül)
     egyébként a MAP karaktere, trip_ch-n át (hallucinálva véletlen tárgyjel, de csak ha az akció időt vitt)
     sötét szobában, seefloor ki: a FLOOR helyett ' '
  vakon csak a játékos mezőjét rajzolja, de a szomszédos szörnyek ébresztése vakon is lefut

erase_lamp(pos, room):  // csak sötét, nem fantom szobában, seefloor be, nem vak
  a pos körüli 3×3-ban a SCREEN FLOOR jelei ' '-re (a játékos mezője kivételével)
```
- **Ami a SCREEN-en marad (memória):** falak, ajtók, folyosók, lépcső, felfedezett csapdák **és a már látott tárgyak**. Sötét szobából távozva *kizárólag a FLOOR karakterek* törlődnek.
- **Tárgyak:** a látott tárgy emlékként a helyén marad akkor is, ha már nincs fény/rálátás. Az emlék nem frissül: ha közben egy szörny felveszi vagy elmozdítja, a képernyőn a régi jel marad, amíg a játékos újra nem látja a mezőt. Sötét szobában az új tárgy csak akkor jelenik meg, ha a 3×3-as környezetbe kerül; világos szobában belépéskor az összes látszik.
- **Szörnyek:** sosem maradnak emlékként. Csak akkor látszanak, ha *éppen* láthatók: világos szobában a játékossal azonos szobában, sötét szobában és folyosón csak a 3×3-ban (dist2 < 3), egyébként SEEMONST/CANSEE szabályai szerint. Ha nem látható, a helyén az alatta lévő emlékezett mező (`oldch`) jelenik meg.
- Rejtett (F_REAL nélküli) mezők a rejtett alakjukban jelennek meg, amíg fel nem fedezik őket.
- A láthatóság logikai fogalom: egy szörny akkor is „látható”, ha a mezője éppen kívül esik a nézőablakon ([3.5](#35-nézőablak-görgetés)). A nézőablak csak a kirajzolást vágja, a játékszabályokat nem befolyásolja.

## 3.2 Állapotok hatása a kijelzésre
- **ISBLIND:** a `look` csak a játékos saját mezőjét frissíti, szoba nem világosodik ki, szörny nem látszik (`see_monst` hamis). Visszanyeréskor (`sight`): ha a játékos szobában van, `enter_room`; „the veil of darkness lifts” (hallucinálva: „far out!  Everything is all cosmic again”).
- **ISHALU** – két része van:
  - `look` közben (`trip_ch`): minden látott mező, ami nem padló, üres, folyosó, fal, ajtó vagy csapda – és nem a már látott lépcső –, véletlen tárgyjelként (`rnd_thing`, [6.2](06_szornyek.md#62-generálás)) rajzolódik; a látható szörnyek véletlen nagybetűként.
  - minden kör elején (`visuals` démon, BEFORE fázis; futás közben `jump` mellett kimarad): a látható tárgyak, a még nem látott lépcső és a látható szörnyek újrasorsolódnak (az álcázott Xeroc tárgyjelet kap); SEEMONST mellett a falon túli szörnyek is véletlen betűt kapnak. Ez a teljes szintre vonatkozik, nem csak a nézőablakra (a dobások száma így nem függ a nézőablak helyzetétől).
  - Lejáratkor (`come_down`): a látható tárgyak és szörnyek a valódi jelükkel rajzolódnak vissza; „Everything looks SO boring now.” Ha a játékos épp vak, csak a flag törlődik: nincs visszarajzolás és üzenet sem.
- **SEEMONST** (monster detection): bekapcsoláskor minden szörny a valódi típusjelével (hallucinálva véletlen betűvel) rajzolódik, a nem látottak inverzen; lejártakor a nem látott szörnyek helyére az `oldch` kerül. Ha egyetlen nem látott szörny sincs: „you have a strange feeling for a moment, then it passes” (hallucinálva „normal”).
- **CANSEE:** bekapcsoláskor (`invis_on`) a látható ISINVIS szörnyek kirajzolódnak (hallucinálva nem); kikapcsoláskor (`unsee`) helyükre az `oldch` kerül. A CANSEE két forrásból állhat fenn (bájital-időzítő, gyűrű), és csak akkor kapcsol ki, ha egyik sincs már (J5, [1.2.1](01_adatmodell_es_flagek.md#121-játékos)).
- **Mágia-észlelés:** külön képernyőn a szint és a szörnyek csomagjainak mágikus tárgyai `$` jellel; „You sense the presence of magic on this level.--More--”, billentyűre visszaáll; nem telik idő.
- **Étel-észlelés:** ugyanígy, az ételek `:` jellel ([8.3](08_targyak.md#83-tekercsek-r)).
- **Térképezés:** a MAP adott részei közvetlenül a SCREEN-re ([8.3](08_targyak.md#83-tekercsek-r)).
- A hallucinált üzenetváltozatok (`choose_str`): több üzenetnek van „trippy” párja (pl. „you feel less trippy now”, „the munchies are interfering with your motor capabilities”); ezeket az adott üzenetnél zárójelben jelöli a specifikáció, ahol eltérnek.

> **C64-eltérés (E11) – észlelés-képernyők:** a 80 oszlopos szint két félben jelenik meg. A térképterület (1..22. sor) törlődik, és először a bal fél (x = 0..39) jelei rajzolódnak ki a képernyő 0..39. oszlopába, az üzenettel és `--More--`-val; szóközre a jobb fél (x = 40..79) következik ugyanígy, a második félnél az üzenet helyén csak `--More--`. Ha az egyik félben nincs egyetlen jel sem, az a fél kimarad (az üzenet ilyenkor a megmaradt félnél jelenik meg). A végén a térkép a SCREEN-ből visszarajzolódik. A fejlesztői mód teljes térképe nem ilyen: az a nézőablakkal görgethető.

## 3.3 Státuszsor

> **C64-eltérés (E3):** az eredeti egysoros, kb. 75 karakteres státuszsor két 40 karakteres sorra oszlik, és a mezők közti szóközök elmaradnak.

A 23. és 24. sorban:
```
23. sor: printf("Level:%-3d Gold:%-6d Hp:%*d(%*d) %s", level, gold, hpw, hpt, hpw, maxhp, hunger_word)
24. sor: printf("Str:%2d(%d) Arm:%-2d Exp:%d/%d", str, str_max, 10 - player_armor(), lvl, exp)
// hpw = a maxhp számjegyeinek száma (igazítás); hunger_word: "", "Hungry", "Weak", "Faint"
// player_armor(): 5.5 – a viselt páncél (vagy az alap 10) a védelem-gyűrűkkel együtt (J11)
pl.  "Level:3   Gold:25     Hp:14(20) Hungry"
     "Str:16(16) Arm:5  Exp:2/38"
```
Mindkét sor a végéig törlődik; ha egy szélsőséges érték (pl. 7 jegyű arany) miatt 40 karakternél hosszabb lenne, a vége levágódik. Csak akkor rajzolódik újra, ha valamelyik érték változott. A `@` parancs a két sort az üzenetsávba írja (egymás után, egy üzenetként).

> **C64-javítás (J11):** a kijelzett Arm a védelem-gyűrűket is tartalmazza, vagyis azt az értéket mutatja, amellyel a harc számol ([5.5](05_jatekos.md#55-effektív-páncél-védekezéskor)). *Eredetileg* a státuszsor csak a viselt páncélt (vagy az alap 10-et) mutatta, a gyűrűk nélkül.

## 3.4 Üzenetsor

> **C64-eltérés (E2):** az üzenetsáv a 0. sor, és ha a szöveg nem fér el benne, az 1. sorra is átnyúlik (kétsoros sáv). Az 1. sor ilyenkor ideiglenesen eltakarja a térkép legfelső sorát.

- Az üzenetsáv két, egyenként 40 karakteres sor, **szótördeléssel**: a szöveg az utolsó olyan szóköznél törik, amely után a sor már nem férne el (40 karakternél hosszabb szó keményen törik). A két sor együtt egy 80 karakteres helynek felel meg, mint az eredeti üzenetsor.
- Minden üzenet nagybetűvel kezdődik (kivéve a csomagbetűvel kezdődőket, pl. „a) …”).
- Az egy körben keletkező üzenetek összefűződnek. Ha az eddigi szöveg és az új rész együttes hossza eléri a 71 karaktert (`MAXMSG = 80 − sizeof("--More--")`, a sizeof a záró nullát is számolja), **vagy** a tördelés miatt a `--More--` már nem férne el a két sorban, az eddigi szöveg után `--More--` jelenik meg, és a játék szóközre vár; utána a sáv törlődik, és az új rész a 0. sor elején kezdődik.
- A következő billentyűleütéskor az üzenetsáv törlődik. `CTRL-P` az utolsó üzenetet ismétli.
- Amíg az 1. sort üzenet foglalja, a térkép 1. sorába rajzolás csak a SCREEN rétegbe kerül, a képernyőre nem; a sáv törlésekor az 1. sor a SCREEN-ből visszarajzolódik.
- **Kérdések és bevitel** (`get_item`, `c`, mentési fájlnév, név): a kérdés szövege ugyanígy a sávba kerül, a beírt szöveg közvetlenül utána, tördelés nélkül folytatódik (ha a 0. sor végére ér, az 1. sorban). Ha a bevitel a 2. sor végére érne, a beviteli mező nem nő tovább (a hosszkorlátokat lásd [1.4.4](01_adatmodell_es_flagek.md#144-szövegek)). DEL az utolsó karaktert törli, RETURN befejezi, ESC megszakít.

## 3.5 Nézőablak (görgetés)

> **C64-eltérés (E1):** a 80 oszlopos logikai térképből a képernyőn egyszerre 40 oszlop látszik. Az ablak a játékost követi, holt zónával.

```
VIEW_W        = 40      // a nézőablak szélessége
SCROLL_MARGIN = 8       // ennyi oszlopnál közelebb a széléhez görget
view_x ∈ 0..40          // a képernyő 0. oszlopában látszó logikai oszlop

center_view():
  view_x = clamp(player.x - 20, 0, 40)
  redraw_map()                                  // az 1..22. sor újrarajzolása a SCREEN-ből

follow_view():  // minden olyan lépés után, amely a játékos pozícióját megváltoztatta (mozgás, teleport)
  sx = player.x - view_x
  if (sx < SCROLL_MARGIN and view_x > 0) or (sx > VIEW_W - 1 - SCROLL_MARGIN and view_x < 40):
     center_view()
```
- `center_view` hívódik új szinten ([2](02_szintgeneralas.md)), betöltés után és `CTRL-R`-re; `follow_view` minden lépés után. Görgetéskor a teljes térképterület újrarajzolódik (22 × 40 karakter). A görgetés karakterenként történik, finom (pixeles) görgetés nem kell.
- **Futás `jump` mellett:** amíg a futás tart, nincs rajzolás; a futás végén `follow_view`, majd a térkép frissítése. `jump` nélkül minden lépés után `follow_view` fut.
- **Rajzolás:** minden SCREEN-írás (`put_screen(x, y, ch)`) a SCREEN rétegbe kerül, és ha `view_x ≤ x < view_x + 40`, egyúttal a videomemória `y`. sorának `x − view_x`. oszlopába is (kivéve az üzenet által foglalt 1. sort, [3.4](#34-üzenetsor)).
- **Animációk** (dobott tárgy, sugár, [7.4](07_harc.md#74-hajítás-t), [7.6](07_harc.md#76-elemi-sugár-lightning--fire--cold-pálca-sárkánytűz)): a nézőablakon kívül eső lépések nem rajzolódnak, és nincs utánuk várakozás sem.
- **Ideiglenes ablakok** (csomaglista *overwrite* módban, észlelés-képernyők, súgó, beállítások): csak a videomemóriába rajzolnak; bezáráskor a képernyő a SCREEN-ből és a státuszsorból visszaáll. Közben a játék nem halad.
