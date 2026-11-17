#include "corridors.h"

#include <stdbool.h>

#include "map.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"

// az irányok a szomszéd indexe szerint növekvő sorrendben
#define DIRECTION_UP     0
#define DIRECTION_LEFT   1
#define DIRECTION_RIGHT  2
#define DIRECTION_DOWN   3
#define DIRECTION_COUNT  4

#define OPPOSITE_DIRECTION(direction)  (DIRECTION_COUNT - 1 - (direction))     // fel <-> le, balra <-> jobbra

// a lépés irányonként, a fenti sorrendben
static const int8_t direction_steps_x[DIRECTION_COUNT] = { 0, -1, 1, 0 };
static const int8_t direction_steps_y[DIRECTION_COUNT] = { -1, 0, 0, 1 };

// a folyosórendszer sorszáma a mezőflagek alsó bitjeibe kerül
#if MAX_PASSAGES > TILE_PASSAGE_NUMBER
    #error "túl sok folyosórendszer: a sorszám nem fér el a TILE_PASSAGE_NUMBER bitjein"
#endif

#define FILL_START  DIRECTION_COUNT             // a kitöltés kezdőmezője: innen nincs hová visszalépni

#define NO_NEIGHBOR  0xff                       // a rács szélén abban az irányban nincs szoba

#define SECRET_PASSAGE_DEPTH_DIVISOR   10       // a folyosó-mező titkos lehet, ha rnd(SECRET_PASSAGE_DEPTH_DIVISOR) + 1 < mélység (default: 10)
#define SECRET_PASSAGE_CHANCE_DIVISOR  40       // és akkor 1 / SECRET_PASSAGE_CHANCE_DIVISOR eséllyel az (default: 40)

// a két dobás egyben: a mező min(mélység - 1, SECRET_PASSAGE_DEPTH_DIVISOR) / SECRET_PASSAGE_ROLL_RANGE eséllyel titkos
#define SECRET_PASSAGE_ROLL_RANGE  (SECRET_PASSAGE_DEPTH_DIVISOR * SECRET_PASSAGE_CHANCE_DIVISOR)   // default: 400

#define SECRET_DOOR_DEPTH_DIVISOR  10          // az ajtó titkos lehet, ha rnd(SECRET_DOOR_DEPTH_DIVISOR) + 1 < mélység (default: 10)
#define SECRET_DOOR_CHANCE_DIVISOR  5           // és akkor 1 / SECRET_DOOR_CHANCE_DIVISOR eséllyel az (default: 5)

// a két dobás egyben: az ajtó min(mélység - 1, SECRET_DOOR_DEPTH_DIVISOR) / SECRET_DOOR_ROLL_RANGE eséllyel titkos
#define SECRET_DOOR_ROLL_RANGE  (SECRET_DOOR_DEPTH_DIVISOR * SECRET_DOOR_CHANCE_DIVISOR)   // default: 50

// csak a generálás alatt kell, ezért nincs a GameState-ben
static uint8_t connected_directions[ROOM_COUNT];   // szobánként a már összekötött irányok, irányonként egy bit
static bool rooms_in_tree[ROOM_COUNT];             // a szoba benne van-e már a feszítőfában

// a szomszédos szoba indexe az adott irányban a szektorrácson
static uint8_t get_neighbor_room(uint8_t room_index, uint8_t direction)
{
    uint8_t column = room_index % SECTOR_GRID_SIZE;

    switch (direction) {
        case DIRECTION_UP:
            return room_index >= SECTOR_GRID_SIZE ? room_index - SECTOR_GRID_SIZE : NO_NEIGHBOR;
        case DIRECTION_LEFT:
            return column > 0 ? room_index - 1 : NO_NEIGHBOR;
        case DIRECTION_RIGHT:
            return column < SECTOR_GRID_SIZE - 1 ? room_index + 1 : NO_NEIGHBOR;
        default:
            return room_index < ROOM_COUNT - SECTOR_GRID_SIZE ? room_index + SECTOR_GRID_SIZE : NO_NEIGHBOR;
    }
}

// egyenletes választás a jelöltek közül egyetlen dobással; egy jelöltnél nincs mit dobni
static uint8_t choose_candidate(const uint8_t *candidates, uint8_t candidate_count)
{
    if (candidate_count == 1) {
        return candidates[0];
    }
    return candidates[rnd(candidate_count)];
}

void put_passage(uint8_t x, uint8_t y)
{
    add_map_flags(x, y, TILE_PASSAGE);
    set_map_character(x, y, CHAR_PASSAGE);
}

void hide_passages(uint8_t kept_x, uint8_t kept_y)
{
    // az esély a mélységgel nő, SECRET_PASSAGE_DEPTH_DIVISOR szint után már nem; az 1. szinten 0, ott nincs dobás
    uint8_t secret_limit = game.dungeon_level - 1;
    if (secret_limit > SECRET_PASSAGE_DEPTH_DIVISOR) {
        secret_limit = SECRET_PASSAGE_DEPTH_DIVISOR;
    }
    if (secret_limit == 0) return;

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            // csak a szabad folyosó-mező: a lépcső jele már nem CHAR_PASSAGE
            if (!(get_map_flags(x, y) & TILE_PASSAGE)) continue;
            if (get_map_character(x, y) != CHAR_PASSAGE) continue;
            if (x == kept_x && y == kept_y) continue;

            // a titkos folyosó-mező üresnek látszik
            if (rnd(SECRET_PASSAGE_ROLL_RANGE) < secret_limit) {
                set_map_character(x, y, CHAR_SOLID_ROCK);
                remove_map_flags(x, y, TILE_REAL);
            }
        }
    }
}

// ajtó a szoba falán, a folyosó végén; a labirintusnál nincs ajtó, ott a kijárat folyosó-mező;
// mélyebben titkos is lehet: akkor a fal jele látszik, és a mező rejtett
static void put_door(const struct Room *room, uint8_t x, uint8_t y)
{
    if (room->flags & ROOM_MAZE) return;

    // az esély a mélységgel nő, SECRET_DOOR_DEPTH_DIVISOR szint után már nem; az 1. szinten 0, ott nincs dobás
    uint8_t secret_limit = game.dungeon_level - 1;
    if (secret_limit > SECRET_DOOR_DEPTH_DIVISOR) {
        secret_limit = SECRET_DOOR_DEPTH_DIVISOR;
    }

    if (secret_limit > 0 && rnd(SECRET_DOOR_ROLL_RANGE) < secret_limit) {
        bool is_horizontal_wall = y == room->y || y == room->y + room->height - 1;
        set_map_character(x, y, is_horizontal_wall ? CHAR_WALL_HORIZONTAL : CHAR_WALL_VERTICAL);
        remove_map_flags(x, y, TILE_REAL);
    } else {
        set_map_character(x, y, CHAR_DOOR);
    }
}

// a labirintus kijárata (J18): a sor vagy az oszlop (step_x, step_y: 1, 0 vagy 0, 1) folyosó-mezői közül, a teljes hosszban,
// egyetlen dobással; egy mezőnél nincs mit dobni; a visszatérés a mező távolsága az első mezőtől
static uint8_t choose_maze_exit(uint8_t first_x, uint8_t first_y, uint8_t step_x, uint8_t step_y, uint8_t length)
{
    uint8_t passage_count = 0;
    for (uint8_t offset = 0; offset < length; offset++) {
        if (get_map_flags(first_x + offset * step_x, first_y + offset * step_y) & TILE_PASSAGE) {
            passage_count++;
        }
    }

    // a labirintus összefüggő, ezért mindig van ilyen mező (2.6)
    uint8_t chosen = 0;
    if (passage_count > 1) {
        chosen = rnd(passage_count);
    }

    for (uint8_t offset = 0; offset < length; offset++) {
        if (get_map_flags(first_x + offset * step_x, first_y + offset * step_y) & TILE_PASSAGE) {
            if (chosen == 0) return offset;
            chosen--;
        }
    }
    return 0;
}

// a kijárat az ajtótábla végére; a tábla nem telhet be, mert egy szinten legfeljebb MAX_CORRIDORS folyosó van, és mindegyiknek két vége (state.h);
// a folyosórendszer sorszámát később a számozás írja be
static void add_exit(uint8_t room_index, uint8_t x, uint8_t y)
{
    struct Exit *new_exit = &game.exits[game.exit_count];

    new_exit->x = x;
    new_exit->y = y;
    new_exit->room = room_index;
    new_exit->passage_number = NO_PASSAGE_NUMBER;
    game.exit_count++;
}

// a folyosó vége: a fantom szobánál maga a csomópont folyosó-mező, máskor ajtó (labirintusnál a kijárat folyosó-mezője), és az ajtótáblába kerül
static void put_corridor_end(uint8_t room_index, uint8_t x, uint8_t y)
{
    const struct Room *room = &game.rooms[room_index];

    if (room->flags & ROOM_PHANTOM) {
        put_passage(x, y);
    } else {
        add_exit(room_index, x, y);
        put_door(room, x, y);
    }
}

// folyosó a szoba és a jobb oldali vagy az alatta lévő szomszédja közt: a fő irányban halad,
// egy véletlen helyen oldalra fordul, amíg a célajtó vonalába nem ér, aztán megy tovább a fő irányban
static void dig_corridor(uint8_t from_index, uint8_t direction)
{
    uint8_t to_index = get_neighbor_room(from_index, direction);
    const struct Room *from = &game.rooms[from_index];
    const struct Room *to = &game.rooms[to_index];

    // a fantom szobánál a végpont maga a csomópont
    uint8_t start_x = from->x;
    uint8_t start_y = from->y;
    uint8_t end_x = to->x;
    uint8_t end_y = to->y;

    int8_t step_x;                              // a fő irány
    int8_t step_y;
    int8_t turn_step_x;                         // a kanyar iránya
    int8_t turn_step_y;
    uint8_t length;                             // ennyi mező a fő irányban a két vég között
    uint8_t turn_length;                        // ennyi mező a kanyarban

    if (direction == DIRECTION_DOWN) {
        // ajtó a kiinduló szoba alsó falán és a célszoba felső falán, sarok nélkül; labirintusnál kijárat az alsó, illetve a felső sorában
        if (from->flags & ROOM_MAZE) {
            start_y = from->y + from->height - 1;
            start_x = from->x + choose_maze_exit(from->x, start_y, 1, 0, from->width);
        } else if (!(from->flags & ROOM_PHANTOM)) {
            start_x = from->x + rnd(from->width - 2) + 1;
            start_y = from->y + from->height - 1;
        }
        if (to->flags & ROOM_MAZE) {
            end_x = to->x + choose_maze_exit(to->x, to->y, 1, 0, to->width);
        } else if (!(to->flags & ROOM_PHANTOM)) {
            end_x = to->x + rnd(to->width - 2) + 1;
        }

        step_x = 0;
        step_y = 1;
        length = end_y - start_y - 1;
        turn_step_x = start_x < end_x ? 1 : -1;
        turn_step_y = 0;
        turn_length = start_x < end_x ? end_x - start_x : start_x - end_x;
    } else {
        // ajtó a kiinduló szoba jobb oldalfalán és a célszoba bal oldalfalán, sarok nélkül; labirintusnál kijárat a jobb, illetve a bal oszlopában
        if (from->flags & ROOM_MAZE) {
            start_x = from->x + from->width - 1;
            start_y = from->y + choose_maze_exit(start_x, from->y, 0, 1, from->height);
        } else if (!(from->flags & ROOM_PHANTOM)) {
            start_x = from->x + from->width - 1;
            start_y = from->y + rnd(from->height - 2) + 1;
        }
        if (to->flags & ROOM_MAZE) {
            end_y = to->y + choose_maze_exit(to->x, to->y, 0, 1, to->height);
        } else if (!(to->flags & ROOM_PHANTOM)) {
            end_y = to->y + rnd(to->height - 2) + 1;
        }

        step_x = 1;
        step_y = 0;
        length = end_x - start_x - 1;
        turn_step_x = 0;
        turn_step_y = start_y < end_y ? 1 : -1;
        turn_length = start_y < end_y ? end_y - start_y : start_y - end_y;
    }

    // hol fordul a folyosó, a hátralévő hossz szerint
    uint8_t turn_at = rnd(length - 1) + 1;

    put_corridor_end(from_index, start_x, start_y);
    put_corridor_end(to_index, end_x, end_y);

    uint8_t x = start_x;
    uint8_t y = start_y;
    while (length > 0) {
        x += step_x;
        y += step_y;

        if (length == turn_at) {
            for (uint8_t i = 0; i < turn_length; i++) {
                put_passage(x, y);
                x += turn_step_x;
                y += turn_step_y;
            }
        }

        put_passage(x, y);
        length--;
    }

    // a következő lépés pontosan a végpontra érne: a length, a turn_at és a turn_length számítása ezt garantálja;
    // ha ezek változnak, erre figyelni kell, különben a folyosó nem ér el a végpontig
}

// a szoba és az adott irányban lévő szomszédja összekötve, folyosóval
static void connect_neighbor_rooms(uint8_t room_index, uint8_t direction)
{
    uint8_t neighbor_index = get_neighbor_room(room_index, direction);

    connected_directions[room_index] |= 1 << direction;
    connected_directions[neighbor_index] |= 1 << OPPOSITE_DIRECTION(direction);

    // a folyosó a kisebb indexű szobától indul: jobbra vagy lefelé
    if (direction == DIRECTION_UP || direction == DIRECTION_LEFT) {
        dig_corridor(neighbor_index, OPPOSITE_DIRECTION(direction));
    } else {
        dig_corridor(room_index, direction);
    }
}

void connect_rooms(void)
{
    game.exit_count = 0;

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        connected_directions[room_index] = 0;
        rooms_in_tree[room_index] = false;
    }

    uint8_t candidates[DIRECTION_COUNT];        // a választható irányok
    uint8_t candidate_count;

    // feszítőfa: a kiinduló szoba addig marad ugyanaz, amíg van fán kívüli szomszédja, csak utána választ a fából újat
    uint8_t from_room = rnd(ROOM_COUNT);
    rooms_in_tree[from_room] = true;
    uint8_t tree_room_count = 1;

    while (tree_room_count < ROOM_COUNT) {
        candidate_count = 0;
        for (uint8_t direction = 0; direction < DIRECTION_COUNT; direction++) {
            uint8_t neighbor_index = get_neighbor_room(from_room, direction);
            if (neighbor_index != NO_NEIGHBOR && !rooms_in_tree[neighbor_index]) {
                candidates[candidate_count] = direction;
                candidate_count++;
            }
        }

        if (candidate_count == 0) {
            from_room = rnd(ROOM_COUNT);
            while (!rooms_in_tree[from_room]) {
                from_room = rnd(ROOM_COUNT);
            }
            continue;
        }

        uint8_t direction = choose_candidate(candidates, candidate_count);
        uint8_t to_room = get_neighbor_room(from_room, direction);
        rooms_in_tree[to_room] = true;
        connect_neighbor_rooms(from_room, direction);
        tree_room_count++;
    }

    // extra élek: néhány további folyosó még össze nem kötött szomszédok közt, hogy ne mindig egyetlen út legyen
    uint8_t extra_count = rnd(MAX_EXTRA_CORRIDORS + 1);
    for (uint8_t i = 0; i < extra_count; i++) {
        from_room = rnd(ROOM_COUNT);

        candidate_count = 0;
        for (uint8_t direction = 0; direction < DIRECTION_COUNT; direction++) {
            if (get_neighbor_room(from_room, direction) != NO_NEIGHBOR && !(connected_directions[from_room] & (1 << direction))) {
                candidates[candidate_count] = direction;
                candidate_count++;
            }
        }

        if (candidate_count > 0) {
            connect_neighbor_rooms(from_room, choose_candidate(candidates, candidate_count));
        }
    }
}

// a folyosórendszerek kitöltése a három réteget (MAP karakter, MAP flag, SCREEN) egyetlen mezőindexszel éri el: (y - MAP_TOP_ROW) * MAP_COLUMNS + x;
// így a szomszéd vizsgálatában nincs sorszorzás és függvényhívás
static uint8_t *const character_tiles = &game.map_characters[0][0];
static uint8_t *const flag_tiles = &game.map_flags[0][0];
static uint8_t *const seen_tiles = &game.seen_map[0][0];

// a mezőindex változása irányonként
static const int16_t direction_tile_steps[DIRECTION_COUNT] = { -MAP_COLUMNS, -1, 1, MAP_COLUMNS };

// az ajtó bejegyzése az ajtótáblában megkapja a folyosórendszer sorszámát; minden ajtó benne van, mert csak folyosóvégen van ajtó (put_corridor_end)
static void set_exit_passage_number(uint8_t x, uint8_t y, uint8_t passage_number)
{
    for (uint8_t i = 0; i < game.exit_count; i++) {
        struct Exit *entry = &game.exits[i];
        if (entry->x == x && entry->y == y) {
            entry->passage_number = passage_number;
            return;
        }
    }
}

// a folyosórendszer kitöltése a kezdőmezőtől (egy kijárattól, amely már megkapta a sorszámot), ortogonálisan, verem nélkül:
// minden bejárt mező a látott térképen tartja számon, melyik irányból érkezett; ha minden irányát kipróbálta, arra lép vissza,
// és a látott térképen a mező újra üres lesz; a generálás alatt a látott térkép másra nem kell;
// a szomszéd vizsgálata egy szinten több ezerszer fut, ezért nincs benne függvényhívás, csak az ajtónál
static void fill_passage(uint8_t x, uint8_t y, uint8_t passage_number)
{
    uint16_t tile = (uint16_t)(y - MAP_TOP_ROW) * MAP_COLUMNS + x;
    seen_tiles[tile] = FILL_START;
    uint8_t direction = 0;                      // a mezőn a következő kipróbálandó irány

    while (true) {
        if (direction < DIRECTION_COUNT) {
            uint8_t next_x = x + direction_steps_x[direction];
            uint8_t next_y = y + direction_steps_y[direction];
            uint16_t next_tile = tile + direction_tile_steps[direction];
            direction++;

            // a térképen kívül: a 0. oszlop (sor) előtti 255 is
            if (next_x >= MAP_COLUMNS || (uint8_t)(next_y - MAP_TOP_ROW) >= MAP_ROWS) continue;

            // már sorszámozott (ahonnan érkezett, az is)
            uint8_t flags = flag_tiles[next_tile];
            if (flags & TILE_PASSAGE_NUMBER) continue;

            // a folyosó-mező (a titkos és a labirintusé is) bekerül; más mező csak akkor, ha ajtó: látható, vagy titkos,
            // ekkor a fal jele látszik, és a mező rejtett
            if (!(flags & TILE_PASSAGE)) {
                uint8_t character = character_tiles[next_tile];
                bool is_door = character == CHAR_DOOR
                    || (!(flags & TILE_REAL) && (character == CHAR_WALL_HORIZONTAL || character == CHAR_WALL_VERTICAL));
                if (!is_door) continue;
                set_exit_passage_number(next_x, next_y, passage_number);
            }

            flag_tiles[next_tile] = flags | passage_number;
            x = next_x;
            y = next_y;
            tile = next_tile;
            seen_tiles[tile] = OPPOSITE_DIRECTION(direction - 1);
            direction = 0;
        } else {
            uint8_t back_direction = seen_tiles[tile];
            seen_tiles[tile] = CHAR_SOLID_ROCK;
            if (back_direction == FILL_START) return;

            // az előző mezőn az az irány jön, amely a most befejezett mező után következik
            x += direction_steps_x[back_direction];
            y += direction_steps_y[back_direction];
            tile += direction_tile_steps[back_direction];
            direction = OPPOSITE_DIRECTION(back_direction) + 1;
        }
    }
}

void assign_passage_numbers(void)
{
    for (uint8_t room_index = ROOM_COUNT; room_index < ROOM_TABLE_SIZE; room_index++) {
        struct Room *passage = &game.rooms[room_index];
        passage->x = 0;
        passage->y = 0;
        passage->width = 0;
        passage->height = 0;
        passage->flags = ROOM_PHANTOM | ROOM_DARK;
    }

    // legfeljebb MAX_PASSAGES rendszer lesz: mindegyik egy folyosó végétől indul, és minden folyosó teljesen egy rendszerbe kerül
    uint8_t passage_number = 0;
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        for (uint8_t i = 0; i < game.exit_count; i++) {
            struct Exit *entry = &game.exits[i];
            if (entry->room != room_index) continue;
            if (get_map_flags(entry->x, entry->y) & TILE_PASSAGE_NUMBER) continue;

            // a kijárat ajtó, vagy a labirintus folyosó-mezője, ennek a bejegyzésében nincs sorszám
            passage_number++;
            add_map_flags(entry->x, entry->y, passage_number);
            if (!(game.rooms[room_index].flags & ROOM_MAZE)) {
                entry->passage_number = passage_number;
            }
            fill_passage(entry->x, entry->y, passage_number);
        }
    }
}
