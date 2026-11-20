# 6. Szörnyek

## 6.1 Típusok

A 26 szörnytípus adatai. A különleges képességeket (spec) a [6.7](#67-speciális-támadások-sikeres-találat-után-ha-nincs-iscanc) írja le.

| | név | carry | flags | lvl | arm | exp | dmg | spec |
|---|---|---|---|---|---|---|---|---|
| A | aquator | 0 | MEAN | 5 | 2 | 20 | 0x0/0x0 | rozsda |
| B | bat | 0 | FLY | 1 | 3 | 1 | 1x2 | kaotikus 50% |
| C | centaur | 15 | – | 4 | 4 | 17 | 1x2/1x5/1x5 | |
| D | dragon | 100 | MEAN | 10 | −1 | 5000 | 1x8/1x8/3x10 | tűz |
| E | emu | 0 | MEAN | 1 | 7 | 2 | 1x2 | |
| F | venus flytrap | 0 | MEAN | 8 | 3 | 80 | 0x0 (lásd 6.7) | megfog |
| G | griffin | 20 | MEAN,FLY,REGEN | 13 | 2 | 2000 | 4x3/3x5 | |
| H | hobgoblin | 0 | MEAN | 1 | 5 | 3 | 1x8 | |
| I | ice monster | 0 | – | 1 | 9 | 5 | 0x0 | fagyaszt |
| J | jabberwock | 70 | – | 15 | 6 | 3000 | 2x12/2x4 | |
| K | kestrel | 0 | MEAN,FLY | 1 | 7 | 1 | 1x4 | |
| L | leprechaun | 0 | – | 3 | 8 | 10 | 1x1 | aranylopás |
| M | medusa | 40 | MEAN | 8 | 2 | 200 | 3x4/3x4/2x5 | zavaró pillantás |
| N | nymph | 100 | – | 3 | 9 | 37 | 0x0 | tárgylopás |
| O | orc | 15 | GREED | 1 | 6 | 5 | 1x8 | |
| P | phantom | 0 | INVIS | 8 | 3 | 120 | 4x4 | kaotikus 20% |
| Q | quagga | 0 | MEAN | 3 | 3 | 15 | 1x5/1x5 | |
| R | rattlesnake | 0 | MEAN | 2 | 3 | 9 | 1x6 | méreg |
| S | snake | 0 | MEAN | 1 | 5 | 2 | 1x3 | |
| T | troll | 50 | REGEN,MEAN | 6 | 4 | 120 | 1x8/1x8/2x6 | |
| U | black unicorn | 0 | MEAN | 7 | −2 | 190 | 1x9/1x9/2x9 | |
| V | vampire | 20 | REGEN,MEAN | 8 | 1 | 350 | 1x10 | HP-szívás |
| W | wraith | 0 | – | 5 | 4 | 55 | 1x6 | szintszívás |
| X | xeroc | 30 | – | 7 | 7 | 100 | 4x4 | álcázás |
| Y | yeti | 30 | – | 4 | 6 | 50 | 1x6/1x6 | |
| Z | zombie | 0 | MEAN | 2 | 8 | 6 | 1x8 | |

Minden szörny `str = 10`. A típustábla állandó (ROM-szerű adat), játék közben nem módosul (J4).

## 6.2 Generálás
```
INITIAL = "KEBSHIROZLCQANYFTWPXUMVGJD"
WANDER  = "KEBSH0ROZ0CQA0Y0TWP0UMVGJ0"  // 0 = nem lehet vándorló

new_monster(depth, type_list):  // depth: a dungeon-szint; type_list: INITIAL vagy WANDER
  ha a szörnytábla tele van: nem jön létre (1.4.1) – a hívó ezt kezeli; dobás sem fut
  loop:
    index = depth + rnd(10) - 6               // a mélység körüli véletlen pozíció a listában
    if index < 0:  index = rnd(5)
    if index > 25: index = rnd(5) + 21
    if type_list[index] != 0: break           // 0: ezen a helyen nincs vándorló
  type = TYPES[type_list[index]]
  depth_bonus = max(depth - AMULET_LEVEL, 0)  // az amulett szintje alatt erősebbek a szörnyek
  monster.lvl = type.lvl + depth_bonus
  monster.maxhp = monster.hpt = ROLL(monster.lvl, 8)
  monster.arm = max(type.arm - depth_bonus, -128)  // 8 bites előjeles (1.4.2); csak 150. szint alatt számít
  monster.dmg = type.dmg; monster.str = type.str; monster.flags = type.flags
  monster.exp = type.exp + depth_bonus*10 + expmod(monster.lvl, monster.maxhp)
  if depth > 29: monster.flags |= ISHASTE
  monster.turn = true                         // lassítva az első körben lép
  monster.oldch = a mező jelenlegi képernyőjele
  monster a szörnytábla VÉGÉRE kerül          // a runners hátulról jár, így a legújabbal kezd (1.4.1)
  if aggravate ring worn: runto(monster)      // ISRUN, ISHELD le, dest = find_dest
  monster.disguise = type.letter
  if type.letter == 'X': monster.disguise = rnd_thing()
       // egyenletesen: ! ? = / : ) ] % *   és a 26. szinttől a , (amulett) is
       // (POTION, SCROLL, RING, STICK, FOOD, WEAPON, ARMOR, STAIRS, GOLD, [AMULET])

expmod(lvl, hp):  // az XP-érték HP-tól függő része
  bonus = (lvl == 1) ? hp/8 : hp/6
  if lvl > 9: bonus *= 20 else if lvl > 6: bonus *= 4
  return bonus
```

**Vándorló szörny démon:**
```
// induláskor: időzítő spread(70) (AFTER fázis); lejártakor elindul a rollwand démon (BEFORE fázis)
rollwand():  // BEFORE fázis, körönként egyszer
  if ++wander_check_tick >= 4:  // csak minden 4. körben dob
     if ROLL(1,6) == 4:
        wanderer()
        rollwand démon le; új időzítő spread(70), ezúttal a BEFORE fázisban
     wander_check_tick = 0

wanderer():
  ha a szörnytábla tele van: nem történik semmi (a démon ettől még úgy folytatódik, mintha létrejött volna)
  hely: find_floor(null, monster), ismételve amíg nem a játékos szobája
  monster = new_monster(level, WANDER)
  if SEEMONST: monster kirajzolása inverzen (hallucinálva véletlen betűvel)
  runto(monster)  // ISRUN; dest = find_dest(monster) – általában a játékos
  nem kap tárgyat
```

## 6.3 Kezdő tárgy
```
give_item(monster):
  if level >= max_level and rnd(100) < carry(monster): monster.pack += new_item()  // ha a tárgytábla engedi (1.4.1)
// carry(monster) = a típustábla carry értéke (6.1)
```

## 6.4 Ébresztés

**Mikor hívódik a `wake_monster`:**
1. **Szobába belépéskor és szoba elhagyásakor** (`enter_room` / `leave_room` → `door_open`): a szoba **összes** szörnyére, világos és sötét szobában egyaránt (fantom szobánál nem).
2. **Minden játékos-akció előtt** (`look(TRUE)` a parancsciklusban): a játékos körüli **8 szomszédos** mezőn álló, látható szörnyekre. Átlós szomszéd csak akkor, ha nem sarkon át esik.

```
wake_monster(monster):
  // 1. aljas szörny
  if !monster.ISRUN and rnd(3) != 0 and monster.ISMEAN and !monster.ISHELD
     and !wearing(STEALTH) and !player.ISLEVIT:
       monster.dest = player; monster.flags |= ISRUN
  // 2. Medusa pillantása – minden hívásnál próbálkozik, amíg el nem sütötte (ISFOUND)
  if monster.type == 'M' and !player.ISBLIND and !player.ISHALU
     and !monster.ISFOUND and !monster.ISCANC and monster.ISRUN:
       if (monster.room == player.room and a szoba világos) or dist2(monster, player) < 3:   // J13
          monster.flags |= ISFOUND                      // a mentődobás előtt → csak egy esély
          if !save_magic():
             ISHUH; ha már zavarodott: az időzítő +spread(20), különben új időzítő spread(20)
             név(monster) + "'s gaze has confused you"  // pl. "the medusa's gaze …"; ha név(monster) == "it": "its gaze …"
  // 3. kapzsi szörny – nincs 2/3 esély, a stealth és a lebegés sem védi ki, ISHELD-et sem nézi
  if monster.ISGREED and !monster.ISRUN:
       monster.flags |= ISRUN
       monster.dest = (a JÁTÉKOS szobájában ROOMGOLD arany fekszik) ? az az arany : player   // a padló tárgylistájából (E23)

runto(monster):  // a "kényszerített" ébresztés
  monster.flags |= ISRUN; monster.flags &= ~ISHELD; monster.dest = find_dest(monster)
```

> **C64-javítás (J13):** a pillantáshoz a Medusának látnia kell a játékost: vagy ugyanabban a világos szobában vannak, vagy szomszédosak. *Eredetileg* csak a *játékos* szobájának világosságát nézte, így egy sötét szobában vagy folyosón álló Medusa is elsüthette a pillantását, ha a játékos egy világos szobában állt (pl. szobába lépéskor a fal túloldaláról).

**Nem ISMEAN és nem ISGREED szörny** (B, C, I, J, L, N, P, W, X, Y) a `wake_monster`-től **soha nem ébred fel**, akármilyen közel megy hozzá a játékos, vagy akárhányszor lép be a szobájába. Csak `runto` indítja el, ezek pedig:
- a játékos megtámadja (közelharc, dobott tárgy – `fight()` eleje);
- pálca/bot hat rá (haste/slow monster, teleport away/to, elemi sugár – találatnál és elkerülésnél is);
- aggravate monsters tekercs, vagy aggravate monster gyűrű felhúzása (a szint összes szörnye);
- új szörny születik aggravate gyűrű viselése közben;
- vándorló szörny (azonnal üldöz).

Az ébren lévő szörnyet a Hold monster tekercs állítja meg (ISRUN le, ISHELD be). A `runto` az ISHELD-et is törli.

## 6.5 Célpont
```
find_dest(monster):
  if TYPES[monster.type].carry <= 0 or monster.room == player.room or see_monst(monster):
     return player
  for each item on the level floor (a tárgylista sorrendjében):
     if item is scare monster scroll: continue
     if room(item.pos) == monster.room and rnd(100) < carry(monster):  // tárgyanként külön dobás
        if no other monster has dest == item: return item               // külön ciklusváltozóval (J3)
  return player
```

> **C64-javítás (J3):** a „más szörny célja-e” ellenőrzés külön változóval járja be a szörnytáblát, így a vizsgált szörny nem változik. *Eredetileg* ugyanazt a változót használta: ha egy tárgy foglaltnak bizonyult, a további tárgyakat már a foglaló szörny szobájához hasonlította (a `carry` esély az eredeti szörnyé maradt).
- `dest`-et a `runto` ([6.4](#64-ébresztés)) és a tárgyfelvétel állítja be; szobaváltáskor nem számolódik újra.
- Ha egy tárgy lekerül a padlóról (a játékos vagy egy szörny felveszi), minden szörny, amelyik rá tartott, a játékost veszi célba ([8.9](08_targyak.md#89-csomag), [6.6](#66-mozgás)). Így a `dest` mindig egy padlón fekvő tárgy vagy a játékos.
- Kapzsi szörny: ha tárgy felé tart, de a szobájában már nem fekszik ROOMGOLD arany, a `dest` a játékos lesz; ezt minden lépés elején megnézi ([6.6](#66-mozgás)). A ROOMGOLD aranyat nem veszi fel, hanem ráül; más szörny felveheti.

> **C64-eltérés (E23):** a szoba aranyát a tárgy ROOMGOLD jelzője mutatja ([1.2.3](01_adatmodell_es_flagek.md#123-tárgy)). A szobarekordban nincs aranymező, és a `dest` mindig egy padlón fekvő tárgy vagy a játékos, mert a szörnyrekord csak tárgyindexet tárol ([1.4.8](01_adatmodell_es_flagek.md#148-szörnyrekord)). *Eredetileg* a szoba tárolta az arany helyét és mennyiségét, a kapzsi szörny célja a szoba arany-koordinátája volt, nem a tárgyé (ezért nem vette fel), és a szoba arany-jelzését csak a játékos aranyfelvétele törölte. Ritka esetekben ez másképp viselkedik:
> - ha a kapzsi szörny tárgygyűjtőként (`find_dest`) indult a szoba aranyáért, eredetileg felvette, most ráül;
> - ha a szoba aranyát egy másik szörny veszi fel, eredetileg a szoba továbbra is aranyasnak számított, és a rá tartó kapzsi szörny az üres mezőre ment; most a játékost veszi célba;
> - az aranyat őrző kapzsi szörny célja foglalja az aranyat, így tárgygyűjtő szörny nem választja (J3); eredetileg választhatta.

## 6.6 Mozgás
```
runners():  // az AFTER fázisban, egyszer körönként
  for each monster (a szörnytábla hátulról, a legújabbal kezdve; az index globális, a kör közbeni halált a kivevő függvény igazítja: 1.4.1):
    if monster.ISHELD or !monster.ISRUN: continue
    old_pos = monster.pos
    if move_monst(monster) == MEGHALT: continue  // pl. a játékost támadva eltűnt (nimfa, leprechaun)
    if monster.ISFLY and dist2(player, monster.pos) >= 3: move_monst(monster)
    if monster.ISTARGET volt és monster.pos != old_pos: ISTARGET le, 'f'-harc vége

move_monst(monster):
  if !monster.ISSLOW or monster.turn: do_chase(monster)
  if monster.ISHASTE: do_chase(monster)
  monster.turn = !monster.turn

do_chase(monster):
  if monster.ISGREED and monster.dest != player and a monster.room-ban nem fekszik ROOMGOLD arany:   // a padló tárgylistájából (E23)
     monster.dest = player
  target_room = (monster.dest == player) ? player.room : room(monster.dest)
  current_room = monster.room; on_door = (MAP[monster.pos] == DOOR)
  loop:
    if current_room != target_room:                               // más szobában van a cél: a legjobb kijárat felé
       goal = current_room kijáratai (ajtótábla, 1.4.7) közül a dest-hez (dist2) legközelebbi
       if on_door: current_room = monster.pos folyosórendszere (az ajtó F_PNUM-ja); on_door = false; ismét a ciklus elejére
    else:
       goal = monster.dest
       if monster.type == 'D' and !ISCANC and egy vonalban (vízszintes/függőleges/átló) a játékossal
          and dist2(monster, player) <= 36 and rnd(5) == 0:
          fire_bolt(monster.pos, irány a játékos felé, "flame")   // 7.6
          futás, ismétlés megszakad; regen-számláló = 0; to_death vége, ha nem ez a célpont
          return
    break
  (next_pos, arrived) = chase(monster, goal)
  if arrived:                                                     // chase nem talált jobb mezőt
     if goal == player.pos: return attack(monster)                // 7.1, 6.7
     if goal == monster.dest:
        if monster.dest egy padlón fekvő tárgy, és nem (monster.ISGREED and a tárgy ROOMGOLD):   // a szoba aranyára a kapzsi szörny ráül (E23)
           felveszi a csomagjába, a mező FLOOR/PASSAGE lesz, a többi rá tartó szörny a játékost veszi célba (6.5)
           monster.dest = find_dest(monster)
        if monster.type != 'F': stop = true
  else if monster.type == 'F': return                             // a Venus flytrap soha nem mozdul el
  relocate(monster, next_pos)
  if stop and monster.pos == monster.dest: monster.ISRUN = false  // pl. a kapzsi szörny ráült az aranyra

chase(monster, goal):  // → (best, megérkezett?); best: ahová lépni fog
  if (monster.ISHUH and rnd(5) != 0) or (monster.type == 'P' and rnd(5) == 0) or (monster.type == 'B' and rnd(2) == 0):
     best = rndmove(monster)                                             // lent
     if rnd(20) == 0: monster.ISHUH = false
     best_dist = dist2(best, goal)
  else:
     best = monster.pos; best_dist = dist2(monster.pos, goal); ties = 1  // ties: hány egyformán jó mezőt láttunk
     for each cell in monster 3×3 környezete (a 0. és 23. sor kivételével), x szerint, azon belül y szerint:
        if !diag_ok(monster.pos, cell): continue
        if !step_ok(MAP jel cell-en) or F_MONST(cell): continue          // fal, kő, szörny (az álcázott Xeroc is) nem jó
        if cell-en scare monster tekercs: continue
        dist = dist2(cell, goal)
        if dist < best_dist:  best = cell; best_dist = dist; ties = 1
        else if dist == best_dist and rnd(++ties) == 0: best = cell      // egyenletes választás az egyenlők közül
  return (best, best_dist == 0 or best == player.pos)
```
```
rndmove(who):  // zavarodott mozgás (szörny és játékos is)
  target.y = who.pos.y + rnd(3) - 1  // előbb az y, aztán az x!
  target.x = who.pos.x + rnd(3) - 1
  if target == who.pos: return target
  if !diag_ok(who.pos, target) or !step_ok(target) or target-en scare monster tekercs: return who.pos
  if who szörny and F_MONST(target): return who.pos   // J22
  return target
```
> **C64-javítás (J22):** szörnynél a véletlen lépés a szörnyes mezőt (F_MONST) is kizárja, így az álcázott Xerocét is. *Eredetileg* a `step_ok` csak a képernyőjelet nézte, ezért a zavarodott szörny, a denevér és a phantom ráléphetett az álcázott Xerocra, és két szörny került egy mezőre. A játékos zavarodott lépése nem változik: az álcázott Xerocra lépve megtámadja.
- A szörny akkor támad, ha a legjobb lépés a játékos mezőjére vinne – vagyis ha szomszédos vele (átlósan csak `diag_ok` szerint).
- Ha nincs közelebbi vagy azonos távolságú mező, helyben marad (`best` = saját mező).
- A szörnyek nem lépnek egymásra (a szörny jele nem `step_ok`), tárgyra igen.

**relocate(monster, new_pos):** ha new_pos ≠ monster.pos: a régi mező helyére `monster.oldch` kerül, `monster.room = room(new_pos)`, majd `monster.oldch` = new_pos képernyőjele, a következő módosításokkal (vakon egyik sem érvényes):
- sötét szobában, ha a régi VAGY az új `oldch` FLOOR, akkor ' ' (így egy tárgy emléke is törlődhet);
- különben a 3×3-as környezeten belül a MAP jele.

Ezután a szörny new_pos-ra kerül. A jele kirajzolódik, ha látható (`disguise`), SEEMONST alatt inverzen a típusa.

## 6.7 Speciális támadások (sikeres találat után, ha nincs ISCANC)
```
A aquator: (dmg 0x0) rust_armor(cur_armor)
           rust_armor: if !armor or leather or arm >= 9: nincs hatás
                       else if ISPROT or maintain armor gyűrű: "the rust vanishes instantly"
                       else: arm++; "your armor appears to be weaker now. Oh my!"
F flytrap: (dmg 0x0, J4) player.ISHELD = true; flytrap_grip++; hpt -= 1; if hpt <= 0: death(F)
           mellé ütéskor (7.1), az ISCANC-tól függetlenül: hpt -= flytrap_grip; if hpt <= 0: death(F)
           flytrap_grip egyetlen globális számláló; a fogással együtt nullázódik (J4): a flytrap halálakor (6.8), teleportkor (8.3),
           új szinten (2.) és ha pálca találja el a flytrapot (8.7)
I ice:     (dmg 0x0) player.ISRUN le; ha épp nem dermedt: "you are frozen by " + név(monster) (7.7, J17); turns_unable_to_act += rnd(2)+2
           if turns_unable_to_act > 50: death("hypothermia")
L leprech: gold -= rnd(50+10*level)+2
           if !save_magic(): gold -= 4 × (rnd(50+10*level)+2)  // négy külön dobás
           gold = max(gold, 0); "your purse feels lighter" (ha változott)
           a leprechaun eltűnik a szintről, a nála lévő tárgyak elvesznek
M medusa:  a pillantás nem támadás, hanem a wake_monster része (6.4); a közelharci támadásai normálisan sebeznek
N nymph:   (dmg 0x0) tárgy = egyenletesen egy mágikus tárgy (8.8) a csomagban, ami nincs használatban
           – az Amulett is mágikus, tehát ellophatja!
           if van: elvesz egy darabot (kötegből egyet), a nimfa eltűnik vele; "she stole <tárgy>!"
           else: nincs hatás, a nimfa marad
R rattle:  if !save_poison():
             if !sustain strength gyűrű: chg_str(-1) (5.6); "you feel a bite in your leg and now feel weaker"
             else if !to_death: "a bite momentarily weakens you"
V vampire: if rnd(100) < 30: drained = ROLL(1,3); hpt -= drained; maxhp -= drained
             if hpt <= 0: hpt = 1;  if maxhp <= 0: death; "you suddenly feel weaker"
W wraith:  if rnd(100) < 15:
             if exp == 0: death
             lvl--; if lvl <= 1: lvl = 1, exp = 0 else exp = xp_next[lvl-2]   // az új szint alsó küszöbe (J2)
             drained = ROLL(1,10); hpt -= drained; maxhp -= drained; if hpt <= 0: hpt = 1; if maxhp <= 0: death
             "you suddenly feel weaker"
X xeroc:   amíg álcázott (disguise != 'X') és a játékos nem vak, bármelyik fél támadása leleplezi (disguise = 'X';
           hallucinálva a jele egy véletlen betű, 3.2):
           a játékos támadása: a xeroc ISRUN-t kap (runto); "wait! That's a xeroc!" (7.7);
             közelharcnál a támadás elmarad, de a kör eltelik; dobásnál a dobott támadás lefut (7.4)
           a xeroc támadása: üzenet nélkül leleplezi magát, a támadás normálisan lefut
           vakon nincs leleplezés: mindkét fél támadása normálisan lefut, az álca marad
D dragon:  lásd tűzokádás (7.6)
```

> **C64-javítás (J2):** a szintszívás után az `exp` az új szint alsó küszöbére kerül, így a szint tartósan elveszik (a visszaszerzéséhez újra meg kell szerezni a hiányzó tapasztalatot). *Eredetileg* `exp = xp_next[lvl-1] + 1` lett, ami a régi szint küszöbe fölé esett (az `xp_next` 0-indexű, 5.2), ezért a következő XP-szerzéskor a szint azonnal visszaállt – ráadásul újabb `ROLL(1,10)` maxhp-val, így a szívás hosszú távon akár növelhette is a maxhp-t. A javítás mellékhatása, hogy a 2. szinttől fölfelé eggyel kevesebb szívás halálos: az *n*. szintről *n*, az eredetiben *n* + 1 kellett, mert ott még egy „1. szint, 11 XP” állapot is közbeékelődött.

> **C64-javítás (J4):** a flytrap találata mindig 0 kocka: csak a fogás (−1) és a mellé ütés (−`flytrap_grip`) sebez, a típustábla nem változik. *Eredetileg* a találat a flytrap **típusának** sebzés-szövegét írta át (`"<flytrap_grip>x1"`; kezdetben `"%%%x0"`, a flytrap halála és a fogva tartott játékos teleportja után `"000x0"`). A szörny viszont a születésekor másolatot kapott a típus sebzéséről, így ez a már élő flytrapra nem hatott: a fogva tartó flytrap találata 0 kocka maradt. Az átírt szöveget csak a később született flytrapok örökölték, tartósan. Ha például a játékos fogva tartva hagyta el a szintet, a következő szint flytrapjai `flytrap_grip` kockával sebeztek. A javítás ezt az öröklést szünteti meg; a szokásos játékmenet nem változik.
>
> A fogás bármilyen megszűnése a `flytrap_grip`-et is nullázza, így a játékos pontosan akkor van fogva, amikor `flytrap_grip > 0`. *Eredetileg* a fogás négy megszűnési módja közül csak a flytrap halála és a teleport nullázta a számlálót, az új szint és a pálca csak az ISHELD-et törölte. A megmaradt számláló miatt bármelyik flytrap mellé ütése azonnal ennyit sebzett, akkor is, ha még meg sem fogta a játékost, és a következő fogás ettől az értéktől számolt tovább.

> **C64-javítás (J17):** az Ice monster üzenete „you are frozen by the ice monster”. *Eredetileg* a szöveg „you are frozen by the ” + név volt, és mivel a név maga is „the”-vel kezdődik, „you are frozen by the the ice monster” jelent meg.

## 6.8 Szörny halála
```
kill_monster(monster):
  exp += monster.exp
  if F: player.ISHELD = false; flytrap_grip = 0
  if L and fallpos(monster.pos) talál helyet and level >= max_level:
     gold_amount = rnd(50+10*level)+2; if save_magic(): gold_amount += 4 × (rnd(50+10*level)+2)
     az arany a szörny csomagjába kerül (és vele együtt leesik)
  monster.pack minden tárgya: fall(item) – a szörny 3×3 környezetének egy véletlen üres
     FLOOR/PASSAGE mezőjére (a játékosé kivételével); ha nincs hely, a tárgy elvész
  eltávolítás, a mező helyére monster.oldch
  "you have defeated " + név(monster) (7.7: nem látott szörnynél "something"); drain life-nál csak akkor, ha a szörny látható
  szintlépés-ellenőrzés (5.2)
```
