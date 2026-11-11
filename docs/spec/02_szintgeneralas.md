# 2. Szintgenerálás

Sorrend:
```
new_level():
  player.ISHELD = false
  if level > max_level: max_level = level
  clear MAP, SCREEN, monsters (és csomagjaik), floor items  // a táblák helyei felszabadulnak (1.4.1)
  do_rooms()                    // 2.2–2.5, 2.7: szobák, és szobánként arany + kezdő szörny
  connect_rooms()               // 2.4, 2.6: összekötés és folyosók
  no_food++
  put_things()                  // 2.8: kincses szoba, 9 próba, amulett
  place_traps()                 // 2.9
  place_stairs()                // 2.10; seenstairs = false
  minden szörny: monster.room = room(monster.pos)
  place_player()                // 2.11 + enter_room
  center_view_window()                 // 3.5: a nézőablak a játékosra áll, teljes újrarajzolás
  if SEEMONST: turn_see(false)  // a szörnyek az új szinten is azonnal kirajzolódnak
  if ISHALU: visuals()          // a hallucinált jelek azonnal
```

## 2.1 Szektorok
A térkép `SECTOR_GRID_SIZE × SECTOR_GRID_SIZE` szektorra oszlik, minden szektorba pontosan egy szoba kerül; a szoba indexe a szektor indexe. Az `i`-edik szektor (`i = row * SECTOR_GRID_SIZE + col`, 0..`ROOM_COUNT − 1`):
- egy szektor mérete `sector_size = (SECTOR_WIDTH, SECTOR_HEIGHT)`;
- `sector_corner = ((i % SECTOR_GRID_SIZE) * SECTOR_WIDTH + SECTOR_FIRST_COLUMN, (i / SECTOR_GRID_SIZE) * SECTOR_HEIGHT)` – a szektor bal felső sarka ([1.3](01_adatmodell_es_flagek.md#13-térkép-és-képernyő)).

A méretek képletből jönnek, így egy konstans átírása mindenhol érvényes. Az alapérték az, amivel az eredeti játék (5.4.4) készült; a kódban is kommentként áll a konstans mellett.

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `MAP_COLUMNS` | a logikai térkép oszlopai ([1.3.1](01_adatmodell_es_flagek.md#131-logikai-térkép)) | 80 |
| `MAP_ROWS` | a logikai térkép sorai | 22 |
| `MAP_TOP_ROW` | a térkép első sora; fölötte az üzenetsor | 1 |
| `LOGICAL_SCREEN_ROWS` | `MAP_TOP_ROW + MAP_ROWS + 1`: a térkép az üzenet- és a státuszsorral, mint az eredeti terminálon | 24 |
| `SECTOR_GRID_SIZE` | a rács mérete | 3 |
| `ROOM_COUNT` | `SECTOR_GRID_SIZE × SECTOR_GRID_SIZE` | 9 |
| `SECTOR_WIDTH` | `MAP_COLUMNS / SECTOR_GRID_SIZE`, lefelé kerekítve | 26 |
| `SECTOR_HEIGHT` | `LOGICAL_SCREEN_ROWS / SECTOR_GRID_SIZE` | 8 |
| `SECTOR_FIRST_COLUMN` | a szektorok első oszlopa | 1 |

A szektorok a **logikai** térképre vonatkoznak ([1.3.1](01_adatmodell_es_flagek.md#131-logikai-térkép)), nem a C64 képernyőjére: a szint ugyanolyan, mint az eredetiben, a 40 oszlopos képernyőn a nézőablak mutatja ([3.5](03_latoter_es_megjelenites.md#35-nézőablak-görgetés)).

## 2.2 Szobaflagek
```
for i in 0..ROOM_COUNT-1: rooms[i].flags = 0
repeat rnd(MAX_PHANTOM_ROOMS + 1) times: rooms[rnd_room()].flags |= PHANTOM  // 0..MAX_PHANTOM_ROOMS különböző fantom szoba (a rnd_room csak nem fantom szobát ad)
for each room i (sorrendben):
  if PHANTOM: elhelyezés (2.3), continue
  if rnd(DARK_ROOM_CHANCE_DIVISOR) < level - 1:
     rooms[i].flags |= DARK
     if rnd(MAZE_CHANCE_DIVISOR) == 0: rooms[i].flags = MAZE  // értékadás: a DARK törlődik
  méret (2.3), rajzolás (2.5), arany + kezdő szörny (2.7)

rnd_room():  // véletlen, nem fantom szoba indexe
  repeat candidate = rnd(ROOM_COUNT) until !PHANTOM(candidate)
  return candidate
```

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `PHANTOM_ROOM_LIMIT` | egy szinten legfeljebb ennyi fantom szoba | 3 |
| `MAX_PHANTOM_ROOMS` | a kisebbik a `PHANTOM_ROOM_LIMIT` és a `ROOM_COUNT − 1` közül: legalább egy igazi szoba mindig marad, különben a `rnd_room` végtelen ciklusba futna | 3 |
| `DARK_ROOM_CHANCE_DIVISOR` | a szoba `(level − 1) / DARK_ROOM_CHANCE_DIVISOR` eséllyel sötét | 10 |
| `MAZE_CHANCE_DIVISOR` | a sötét szobából `1 / MAZE_CHANCE_DIVISOR` eséllyel labirintus lesz | 15 |

A labirintus mezői folyosó-mezők (`F_PASS`), ezért a látótér szempontjából folyosóként viselkedik (3×3 látás); a DARK-nak itt nincs jelentősége.

## 2.3 Szobaméret és -hely
A szoba `room.pos` (bal felső sarok, fallal együtt) és `room.size` (méret, fallal együtt) mezője a szektorán belül ([2.1](#21-szektorok)):
```
PHANTOM:  // a fantom szoba nem igazi szoba, csak egyetlen folyosó-csomópont
  // véletlen pont a szektor belsejében, a szektor szélétől legalább PHANTOM_EDGE_DISTANCE mezőre
  room.pos.x = sector_corner.x + rnd(sector_size.x - 2 * PHANTOM_EDGE_DISTANCE) + PHANTOM_EDGE_DISTANCE
  room.pos.y = sector_corner.y + rnd(sector_size.y - 2 * PHANTOM_EDGE_DISTANCE) + PHANTOM_EDGE_DISTANCE
  room.size = (0, 0)  // nincs mérete: minden használat előtt a PHANTOM jelzőt nézzük

MAZE:  // a labirintus kitölti a szektort, az utolsó oszlopa és sora nélkül
  room.size = (sector_size.x - 1, sector_size.y - 1)
  room.pos  = sector_corner
  if room.pos.x == SECTOR_FIRST_COLUMN: room.pos.x = 0  // a bal szélső a 0. oszloptól kezdődik
  if room.pos.y < MAP_TOP_ROW:                          // a felső szektorsorban a térkép fölötti sorok kimaradnak
     room.size.y -= MAP_TOP_ROW - room.pos.y
     room.pos.y = MAP_TOP_ROW
  // a legtávolabbi cella a méret páros része (2.5): ha a térképen kívül esne, a méret a térkép széléig csökken
  if room.pos.x + room.size.x / 2 * 2 > MAP_COLUMNS - 1:            room.size.x = MAP_COLUMNS - 1 - room.pos.x
  if room.pos.y + room.size.y / 2 * 2 > MAP_TOP_ROW + MAP_ROWS - 1: room.size.y = MAP_TOP_ROW + MAP_ROWS - 1 - room.pos.y

normál:
  repeat:
    room.size.x = rnd(sector_size.x - MIN_ROOM_SIZE) + MIN_ROOM_SIZE  // falakkal együtt
    room.size.y = rnd(sector_size.y - MIN_ROOM_SIZE) + MIN_ROOM_SIZE
    room.pos.x  = sector_corner.x + rnd(sector_size.x - room.size.x)  // úgy, hogy beférjen a szektorba
    room.pos.y  = sector_corner.y + rnd(sector_size.y - room.size.y)
  until room.pos.y >= MAP_TOP_ROW                                     // a felső fala nem kerülhet a térkép fölé
```

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `PHANTOM_EDGE_DISTANCE` | a fantom szoba pontja legalább ennyi mezőre van a szektor szélétől | 1 |
| `MIN_ROOM_SIZE` | a normál szoba legkisebb mérete falakkal együtt | 4 |

**A labirintus határa:** a cellák a páros relatív koordinátákon vannak (2.5), így a labirintus legtávolabbi mezője `room.pos + room.size / 2 * 2`. Az alapértékekkel ez mindig a térképen belül van (jobbra legfeljebb 53 + 24 = 77, lefelé 16 + 6 = 22), a vágás soha nem lép életbe, és a 80×24-es labirintusok ugyanolyanok, mint az eredetiben. Más méreteknél kilóghatna: páratlan szektorszélességnél (pl. 81 oszlop: 55 + 26 = 81) a jobb szélen, 1×1-es rácsnál (1 + 22 = 23) a státuszsorba. A kód a két vágást csak akkor fordítja be, ha a konstansokból kiszámolva szükség van rájuk (`MAZE_NEEDS_WIDTH_LIMIT`, `MAZE_NEEDS_HEIGHT_LIMIT`); a fejlesztői build minden MAP-írásnál `DEV_CHECK`-kel ellenőrzi, hogy a mező a térképen van (`map.c`).

A normál szobánál a szektor utolsó oszlopa és sora mindig üresen marad, ez a szektorok közti rés; az alsó szektorsorban ez a státuszsor.

**Fordítási feltételek** (a fejlesztői és a kiadott buildben is `#error`):
- `MIN_ROOM_SIZE ≥ 3`: a szobának kell legalább egy belső mező (`find_floor`), és a falain sarok nélküli hely az ajtónak (2.6);
- `SECTOR_WIDTH ≥ MIN_ROOM_SIZE + 1`: a legkisebb szoba mellett marad a rés;
- `SECTOR_HEIGHT ≥ MIN_ROOM_SIZE + MAP_TOP_ROW + 1`: a felső szektorsorban is kijöhet a térképen belüli hely, különben az újrasorsolás végtelen.

Az alapértékekkel ez legalább 15 oszlopos térképet jelent (3 × 5).

A fantom szoba pontjára (a kódban közvetlenül a `PHANTOM_EDGE_DISTANCE` definíciója után, szintén `#error`):
- `PHANTOM_EDGE_DISTANCE ≥ 1`: a pont a szektor utolsó oszlopa és sora előtt van, így a folyosó hossza legalább 1 (2.6); és a legalsó szektorban sem kerül a státuszsorba (legfeljebb `LOGICAL_SCREEN_ROWS − 1 − PHANTOM_EDGE_DISTANCE`);
- `PHANTOM_EDGE_DISTANCE ≥ MAP_TOP_ROW`: a felső szektorsorban sem kerül a térkép fölé, az üzenetsorba;
- `2 × PHANTOM_EDGE_DISTANCE < SECTOR_WIDTH` és `2 × PHANTOM_EDGE_DISTANCE < SECTOR_HEIGHT`: marad hely a sorsoláshoz, különben a `rnd` tartománya 0 vagy negatív, és a pont kiesik a szektorból.

Az alapértékekkel (1) a pont `y`-ja mindig 1..22, így újrasorsolás nélkül sem kerülhet az üzenet- vagy a státuszsorba. A feltételek ezt fordítási időben garantálják, a korábbi `DEV_CHECK` elhagyható.

## 2.4 Összekötés
Szomszédság: csak ortogonális szomszéd szektorok. A szomszédok a rácsból számolódnak, növekvő indexsorrendben: fölötte `room − SECTOR_GRID_SIZE` (ha nem a felső sorban van), balra `room − 1` (ha nem a bal oszlopban), jobbra `room + 1` (ha nem a jobb oszlopban), alatta `room + SECTOR_GRID_SIZE` (ha nem az alsó sorban). Az alapértékekkel ez az eredeti `adjacent` táblája: 0–1, 0–3, 1–2, 1–4, 2–5, 3–4, 3–6, 4–5, 4–7, 5–8, 6–7, 7–8. A fantom szobák is részt vesznek (folyosó-csomópontként).

Az összekötés egy véletlen feszítőfa a szobák szomszédsági gráfján, utána néhány extra éllel:
```
connect_rooms():
  connected[a][b] = false minden szobapárra                // a és b közt van-e már folyosó
  in_tree[r]      = false minden szobára                   // r benne van-e már a fában
  from_room = rnd(ROOM_COUNT); in_tree[from_room] = true; rooms_in_tree = 1
  while rooms_in_tree < ROOM_COUNT:
    // from_room egy véletlen, még fán kívüli szomszédja (egyenletesen a jelöltek közül)
    candidates = from_room szomszédai, amelyek még nincsenek a fában (a fenti sorrendben)
    if candidates üres:
       repeat from_room = rnd(ROOM_COUNT) until in_tree[from_room]  // nincs ilyen szomszéd: új kiinduló szoba a fából
    else:
       to_room = candidates[count > 1 ? rnd(count) : 0]
       in_tree[to_room] = true
       corridor(from_room, to_room)
       connected[from_room][to_room] = connected[to_room][from_room] = true
       rooms_in_tree++                                     // from_room marad a kiinduló szoba!

  // extra élek: néhány további folyosó még össze nem kötött szomszédok közt
  repeat rnd(MAX_EXTRA_CORRIDORS + 1) times:
    from_room = rnd(ROOM_COUNT)
    candidates = from_room szomszédai, amelyekkel még nincs összekötve
    if candidates nem üres:
       to_room = candidates[count > 1 ? rnd(count) : 0]
       corridor(from_room, to_room)
       connected[from_room][to_room] = connected[to_room][from_room] = true
```
A kiinduló szoba addig marad ugyanaz, amíg van fán kívüli szomszédja; csak utána választ a fából újat.

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `MAX_EXTRA_CORRIDORS` | a feszítőfa után legfeljebb ennyi extra folyosó-próbálkozás (`rnd(MAX_EXTRA_CORRIDORS + 1)`) | 4 (az eredetiben `rnd(5)`) |

**Eltérés az eredetitől (dobássorrend):** az eredeti a `rnd(++candidates) == 0` mintával választ, ez minden jelöltnél dob (az első jelöltet 1/1, a másodikat 1/2, a harmadikat 1/3 eséllyel veszi át). A C64-es változat, mint a labirintus (2.5), előbb összegyűjti a jelölteket, aztán egyetlen `rnd(count)`-tal választ, egy jelöltnél nem dob. Az esélyek ugyanazok, a szintek mások.

Az eredeti a generálás végén a `passnum`-mal megszámozza a folyosóhálózatokat. Ez csak az A változatban kell ([1.4.7](01_adatmodell_es_flagek.md#147-kijáratok-és-folyosók)), a szörnymozgással együtt dől el, és akkor kerül ide.

Átlós lépés ajtóra vagy ajtóról külön tiltás nélkül is lehetetlen: az ajtó két oldalán fal áll, és az átlós lépéshez mindkét ortogonális szomszédnak járhatónak kell lennie.

## 2.5 Szobák rajzolása
```
PHANTOM: nincs rajzolás (a folyosó a room.pos mezőn halad át)
MAZE:    do_maze(room)
normál:  felső és alsó sor (sarkokkal): WALL_HORIZONTAL '-'; bal és jobb oszlop (sarkok nélkül): WALL_VERTICAL '|'
         belső: FLOOR
```
**Labirintus:** rekurzív visszalépéses (DFS) labirintus, 2 mezős lépésekkel; csak folyosó-mezőkből áll, falkarakter nincs. A cellák koordinátái a labirintus bal felső sarkához (`room.pos`) képest értendők, 0..`room.size`.
```
do_maze(room):
  start.y = (rnd(room.size.y) / 2) * 2  // páros kezdőcella; előbb az y, aztán az x
  start.x = (rnd(room.size.x) / 2) * 2
  putpass(room.pos + start)
  dig(start)

dig(cell):  // cell: relatív koordináta a labirintusban
  loop:
    options = 0                                        // ennyi ásható szomszéd van
    for step in [(+2, 0), (-2, 0), (0, +2), (0, -2)]:  // (dx, dy), ebben a sorrendben: előbb vízszintesen, aztán függőlegesen
      next = cell + step
      if next a labirintuson kívül van (next.y < 0, next.y > room.size.y, next.x < 0, next.x > room.size.x): continue
      if a (room.pos + next) mező már folyosó (F_PASS): continue
      options++
      if rnd(options) == 0: chosen = next              // egyenletes választás az ásható szomszédok közül
    if options == 0: return                            // zsákutca: vissza az előző cellához
    putpass(room.pos + a cell és a chosen közti mező)
    putpass(room.pos + chosen)
    dig(chosen)
```
A labirintus is `putpass`-szal készül, így a mezői is lehetnek rejtettek ([2.6](#26-folyosók)).

**A felső és a középső labirintus összeérhet**, ahogy az eredetiben is: a felső szektorsorban a labirintus az 1. sorban kezdődik, ezért a cellái a páratlan sorokra esnek, és az utolsó a szektor utolsó sorába (alapértékekkel a 7.-be) kerül, a középső labirintus első cellasora pedig a 8. sor. A középső és az alsó között a szektor utolsó sora üres marad. Később újragondoljuk: egy ötlet, hogy a 2. és a 3. szektorsorban is eggyel lejjebb kezdődjön a labirintus, így a középső eggyel magasabb lenne, a felső és az alsó pedig egyforma. Ezzel a labirintus kijáratának javítása (J18, [2.6](#26-folyosók)) is elhagyható lenne.

**C64-en a `dig` nem lehet rekurzív**, mert a 6502 hardveres verme 256 bájtos. Az iteratív változat ugyanazokkal az esélyekkel választ, de kevesebb dobással: előbb összegyűjti az ásható szomszédokat, aztán egyetlen `rnd(options)`-szal választ közülük, egy szomszédnál pedig nem dob. A labirintusok ezért mások, mint az eredetiben, de ugyanolyan eloszlásúak:
```
dig_iterative(start):
  stack = [start]                      // saját verem: cellánként 2 bájt (relatív x, y)
  while stack nem üres:
    cell = stack.top
    candidates = a cell ásható szomszédai, a fenti lépéssorrendben
    if candidates üres: stack.pop      // zsákutca: vissza az előző cellához
    else:
      chosen = candidates[options > 1 ? rnd(options) : 0]
      putpass(köztes mező); putpass(chosen); stack.push(chosen)
```
A verem legfeljebb annyi elemű, ahány cella a labirintusban van, mert minden cella legfeljebb egyszer kerül bele: `(room.size.x/2 + 1) × (room.size.y/2 + 1)`. A legnagyobb labirintus a szektor az utolsó oszlopa és sora nélkül, így a verem mérete:

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `MAZE_STACK_SIZE` | `((SECTOR_WIDTH − 1) / 2 + 1) × ((SECTOR_HEIGHT − 1) / 2 + 1)` | 13 × 4 = 52 (az eredetiben nincs, ott a C verem dolgozik) |

A fejlesztői build `DEV_CHECK`-kel ellenőrzi, hogy a verem nem telik túl. 255-nél nagyobb veremnél (pl. 1×1-es rácsnál 40 × 12 = 480) a veremindex 16 bites.

A labirintus generálása lassú: egy labirintus mélyen nagyjából 0,2 másodperc, az 1. szinten kevesebb, mert ott a `putpass` nem dob. Az alapértékekkel labirintus ritkán van egy szinten (a sötét szobák kb. 1/15-e).

## 2.6 Folyosók
Két szomszédos szoba (`room_a`, `room_b`) közti folyosó. A kisebb indexű szobától indul; ha a másik a jobb oldali szomszédja, jobbra halad, ha az alatta lévő, lefelé.
```
corridor(room_a, room_b):
  first = min(room_a, room_b)
  direction = (max(room_a, room_b) == first + 1) ? RIGHT : DOWN
  from = rooms[first]
  to   = rooms[direction == RIGHT ? first + 1 : first + SECTOR_GRID_SIZE]

  if direction == DOWN:
     start = from.pos; end = to.pos                  // fantom szobánál maga a csomópont
     if MAZE(from):                                  // kijárat a labirintus alsó sorában (J18)
        start.y = from.pos.y + from.size.y - 1
        start.x = véletlen folyosó-mező a start.y sorban, from.pos.x .. from.pos.x + from.size.x - 1 közül
     else if !PHANTOM(from):                         // ajtó a kiinduló szoba alsó falán, sarok nélkül
        start.x = from.pos.x + rnd(from.size.x - 2) + 1
        start.y = from.pos.y + from.size.y - 1
     if MAZE(to):                                    // kijárat a labirintus felső sorában (J18); end.y = to.pos.y
        end.x = véletlen folyosó-mező az end.y sorban, to.pos.x .. to.pos.x + to.size.x - 1 közül
     else if !PHANTOM(to):                           // ajtó a célszoba felső falán, sarok nélkül
        end.x = to.pos.x + rnd(to.size.x - 2) + 1    // end.y = to.pos.y
     step = (0, 1)                                   // a fő irány: lefelé
     length = |start.y - end.y| - 1                  // ennyi mező a fő irányban
     turn_step = (start.x < end.x ? +1 : -1, 0)      // a kanyar iránya: jobbra vagy balra
     turn_length = |start.x - end.x|                 // ennyi mező a kanyarban
  else (direction == RIGHT): ugyanez a tengelyek felcserélésével
     (start a kiinduló szoba jobb oldalfalán, end a célszoba bal oldalfalán, step = (1, 0);
      labirintusnál a jobb szélső, illetve a bal szélső oszlop egy véletlen folyosó-mezője)
  turn_at = rnd(length - 1) + 1                      // hol fordul a folyosó (a hátralévő hossz szerint); length >= 1, lásd lent

  PHANTOM(from) ? putpass(start) : door(from, start)
  PHANTOM(to)   ? putpass(end)   : door(to, end)
  pos = start
  while length > 0:
    pos += step
    if length == turn_at:
       repeat turn_length: putpass(pos); pos += turn_step
    putpass(pos); length--
  pos += step  // most pos == end kell legyen (a fejlesztői build DEV_CHECK-kel ellenőrzi; az eredeti itt figyelmeztetett: „connectivity problem”)

door(room, pos):
  exits += (pos, room)                                                // a közös ajtótáblába (1.4.7)
  if MAZE(room): return                                               // labirintusnál nincs ajtó
  if rnd(10) + 1 < level and rnd(5) == 0:
     MAP[pos] = (pos a felső vagy alsó falon) ? '-' : '|'; F_REAL le  // titkos ajtó: falnak látszik
  else: MAP[pos] = DOOR

putpass(pos):
  F_PASS be
  if rnd(SECRET_PASSAGE_DEPTH_DIVISOR) + 1 < level and rnd(SECRET_PASSAGE_CHANCE_DIVISOR) == 0:
     F_REAL le                                        // titkos folyosó-mező (üresnek látszik)
  else: MAP[pos] = PASSAGE
```

| konstans | érték | alapérték (5.4.4) |
|---|---|---|
| `SECRET_PASSAGE_DEPTH_DIVISOR` | a folyosó-mező titkos lehet, ha `rnd(SECRET_PASSAGE_DEPTH_DIVISOR) + 1 < level` | 10 |
| `SECRET_PASSAGE_CHANCE_DIVISOR` | és akkor `1 / SECRET_PASSAGE_CHANCE_DIVISOR` eséllyel az | 40 |
| `SECRET_DOOR_DEPTH_DIVISOR` | az ajtó titkos lehet, ha `rnd(SECRET_DOOR_DEPTH_DIVISOR) + 1 < level` | 10 |
| `SECRET_DOOR_CHANCE_DIVISOR` | és akkor `1 / SECRET_DOOR_CHANCE_DIVISOR` eséllyel az | 5 |

> **C64-javítás (J18):** a labirintus kijárata a falsor (oszlop) folyosó-mezői közül egyetlen dobással választódik, a sor (oszlop) teljes szélességében. *Eredetileg* a sarkok nélküli tartományban (`rnd(size − 2) + 1`) addig dobott, amíg folyosó-mezőt nem talált. A felső szektorsor labirintusa az 1. sorban kezdődik, így az alsó sora páratlan relatív sor, ahol csak a cellasorok közti átkötéseknél van folyosó-mező. Ha mindegyik a szélső oszlopban van (szimulálva a felső labirintusok kb. 0,05%-ánál), a keresés soha nem állt meg. A javítás ideiglenes lehet: ha a labirintusok eggyel lejjebb kezdődnek (lásd az összeérésüket, 2.5), a falsor mindig cellasor, és az eredeti keresés is megáll. A teljes szélességben mindig van folyosó-mező, mert a labirintus összefüggő: a páratlan falsor alatt még van cellasor, és azt csak ezen át lehet elérni. A normál szoba ajtaja változatlanul sarok nélkül, egy dobással kerül a falra.

**A folyosó hossza** (`length`) mindig legalább 1, így a `turn_at` dobása (`rnd(0)` = 0) is értelmes: a szoba falai, a labirintus kijárata és a fantom szoba pontja a szektor utolsó oszlopa és sora előtt vannak, a szomszéd szektoré pedig legkorábban annak első oszlopában, illetve sorában. Ehhez a fantom pontnak legalább 1 mezőre kell lennie a szektor szélétől: ez a `PHANTOM_EDGE_DISTANCE ≥ 1` fordítási feltétel (2.3). Futás közbeni ellenőrzés nem kell: ha a hossz mégis 0 lenne, a folyosó nem érne a végpontba, és azt a végpont `DEV_CHECK`-je jelzi.

**Titkos ajtó egyetlen dobással**, ugyanúgy, mint a titkos folyosó-mező (lent): az ajtó `min(level − 1, SECRET_DOOR_DEPTH_DIVISOR) / (SECRET_DOOR_DEPTH_DIVISOR × SECRET_DOOR_CHANCE_DIVISOR)` eséllyel titkos (mélyen legfeljebb 10/50 = 1/5). Az 1. szinten nincs dobás. **Eltérés az eredetitől (dobássorrend):** az eredeti két dobása helyett egy; az esély ugyanaz.
```
door(room, pos):  // a C64-es változat
  exits += (pos, room)
  if MAZE(room): return
  secret_limit = min(level - 1, SECRET_DOOR_DEPTH_DIVISOR)
  if secret_limit > 0 and rnd(SECRET_DOOR_DEPTH_DIVISOR * SECRET_DOOR_CHANCE_DIVISOR) < secret_limit:
     MAP[pos] = (pos a felső vagy alsó falon) ? '-' : '|'; F_REAL le
  else: MAP[pos] = DOOR
```

A kódban a folyosók (`connect_rooms`, `corridor`, `door`, `put_passage`) a `corridors.c`-ben vannak; a labirintus is innen hívja a `put_passage`-et.

**Titkos folyosó-mező egyetlen dobással:** a `putpass` két dobása helyett egy, ugyanazzal az eséllyel: az első feltétel `min(level − 1, SECRET_PASSAGE_DEPTH_DIVISOR) / SECRET_PASSAGE_DEPTH_DIVISOR` eséllyel teljesül, így a mező együtt `min(level − 1, SECRET_PASSAGE_DEPTH_DIVISOR) / (SECRET_PASSAGE_DEPTH_DIVISOR × SECRET_PASSAGE_CHANCE_DIVISOR)` eséllyel titkos (mélyen legfeljebb 10/400 = 1/40). Az 1. szinten nincs dobás, mert ott az esély 0. **Eltérés az eredetitől (dobássorrend):** a dobások sorrendje ezért eltér; az esély ugyanaz.
```
put_passage(pos):
  F_PASS be
  secret_limit = min(level - 1, SECRET_PASSAGE_DEPTH_DIVISOR)
  if secret_limit > 0 and rnd(SECRET_PASSAGE_DEPTH_DIVISOR * SECRET_PASSAGE_CHANCE_DIVISOR) < secret_limit:
     F_REAL le
  else: MAP[pos] = PASSAGE
```

## 2.7 Arany és kezdő szörnyek (szobánként, nem fantom szoba)
```
for each room (not PHANTOM):
  has_gold = false
  if rnd(2) == 0 and (not has_amulet or level >= max_level):  // a rnd(2) mindig lefut
     room.gold_val = rnd(50 + 10*level) + 2
     room.gold_pos = find_floor(room, item)                   // üres padló (labirintusnál üres járat)
     place GOLD on room.gold_pos
     has_gold = true
  if rnd(100) < (has_gold ? 80 : 25):
     monster = new_monster(level, initial_list=true)
     place monster on find_floor(room, monster)               // bármely járható mező szörny nélkül – lehet aranyon is
     monster.flags = típus flagjei (nincs ISRUN → alszik)
     give_item(monster)  (lásd 6.3)

find_floor(room /*null = véletlen nem fantom szoba minden próbánál*/, mode):
  loop:
    if room param == null: room = rooms[rnd_room()]
    pos.x = room.pos.x + rnd(room.size.x - 2) + 1  // véletlen mező a falakon belül; előbb az x, aztán az y
    pos.y = room.pos.y + rnd(room.size.y - 2) + 1
    mode == item:    return pos, ha MAP[pos] == (MAZE(room) ? PASSAGE : FLOOR)  (nincs rajta semmi)
    mode == monster: return pos, ha a jele járható és nincs rajta szörny (tárgy, lépcső, rejtett csapda lehet)
```
A szörnymódban a járhatóságot a mező jele dönti el, nem a flagjei: járható, ha a jele nem szóköz és nem fal. A titkos folyosó-mező jele szóköz, a titkos ajtóé fal, így ezekre sem szörny, sem a játékos (2.11) nem kerül.

A kezdő szörny alszik, mert `new_monster` csak a típus flagjeit adja (ISRUN nélkül).

## 2.8 Tárgyak (`put_things`)
```
put_things():
  if has_amulet and level < max_level: return  // csak a legmélyebb elért szinten
  if rnd(20) == 0: treasure_room()             // 5% kincses szoba
  repeat 9:
    if rnd(100) < 36:
      item = new_item()                        // 8.1
      place item on find_floor(null, item)     // labirintus is lehet
  if level >= AMULET_LEVEL and not has_amulet:
      place AMULET on find_floor(null, item)
```

**Kincses szoba:**
```
treasure_room():
  room = random non-phantom room (labirintus is lehet)
  inner_area = (room.size.x - 2) * (room.size.y - 2)  // belső terület, falak nélkül
  MIN = 2; MAX = 10
  extra = min(inner_area - MIN, MAX - MIN)            // a MIN feletti véletlen többlet felső határa; rnd(≤0) = 0 → ilyenkor is MIN tárgy
  item_count = rnd(extra) + MIN
  repeat item_count:
     pos = find_floor(room, item) max. 20 próbával
     ha nincs találat: kihagy (J7)
     new_item() a pos-ra
  monster_count = max(rnd(extra) + MIN, item_count + 2)
  monster_count = min(monster_count, inner_area)
  level++                                             // a szörnyek ÉS a tárgyaik is a következő szintről
  repeat monster_count:
     pos = find_floor(room, monster) max. 10 próbával; ha nincs: kihagy
     monster = new_monster(level, initial_list=true); monster.flags |= ISMEAN
     give_item(monster)
  level--
```
Szörnyből mindig legalább kettővel több van, mint a tervezett tárgyszám (`item_count`).

> **C64-javítás (J7):** ha a 20 próba egyike sem talál üres mezőt, a tárgy nem jön létre. *Eredetileg* ilyenkor az előző próba `pos`-a maradt, és két tárgy került egy mezőre, amit a padló egyetlen jele nem tud mutatni.

Tárgy és szörny csak a táblák szabad helyeinek erejéig jön létre ([1.4.1](01_adatmodell_es_flagek.md#141-táblák-és-listák)); ez a 2.7–2.8 minden elhelyezésére vonatkozik.

## 2.9 Csapdák
```
if rnd(10) < level:
  trap_count = min(rnd(level/4) + 1, MAX_TRAPS)                  // MAX_TRAPS = 10
  repeat trap_count:
    repeat pos = find_floor(null, item) until MAP[pos] == FLOOR  // labirintusba nem kerül
    F_REAL le                                                    // rejtett: padlóként látszik
    trap_type[pos] = rnd(8)                                      // TRAPDOOR, ARROW, SLEEP, BEAR, TELEPORT, DART, RUST, MYSTERY (F_TMASK)
```

## 2.10 Lépcső
```
repeat pos = find_floor(null, item)  // bármely nem fantom szoba, labirintus is (ott járat-mező)
until F_REAL(pos)                    // rejtett csapdára nem kerülhet (J16)
MAP[pos] = STAIRS
```

> **C64-javítás (J16):** a `find_floor` csak a karaktert nézi, és a rejtett csapda karaktere FLOOR. *Eredetileg* ezért a lépcső rejtett csapda mezőjére is kerülhetett, a mező flagjei (csapdatípus, F_REAL nélkül) pedig megmaradtak. A javítás a rejtett (F_REAL nélküli) mezőt kihagyja és újra próbál.

## 2.11 Játékos elhelyezése
```
pos = find_floor(null, monster)  // bármely nem fantom szoba (labirintus is), szörny nélküli járható mező
player.pos = pos; player.ISHELD = false
enter_room(pos)                  // megvilágítás + ébresztés (6.4)
```
A játékos tárgyon, aranyon, lépcsőn vagy rejtett csapdán is kezdhet (a csapda csak rálépéskor sül el, a kezdőmezőn nem).
