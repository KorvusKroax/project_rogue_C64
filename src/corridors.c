#include "corridors.h"

#include "dev/dev.h"
#include "map.h"
#include "rng.h"
#include "rooms.h"
#include "state.h"

// az irányok a szomszéd indexe szerint növekvő sorrendben, mint az eredeti szomszédsági táblájában
#define DIRECTION_UP     0
#define DIRECTION_LEFT   1
#define DIRECTION_RIGHT  2
#define DIRECTION_DOWN   3
#define DIRECTION_COUNT  4

#define OPPOSITE_DIRECTION(direction)  (DIRECTION_COUNT - 1 - (direction))     // fel <-> le, balra <-> jobbra

#define NO_NEIGHBOR  0xff                       // a rács szélén abban az irányban nincs szoba

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

// ajtó a szoba falán, a folyosó végén; a labirintusnál nincs ajtó, ott a kijárat folyosó-mező
static void put_door(const struct Room *room, uint8_t x, uint8_t y)
{
    if (room->flags & ROOM_MAZE) return;

    set_map_character(x, y, CHAR_DOOR);
}

// a folyosó vége: a fantom szobánál maga a csomópont folyosó-mező, máskor ajtó
static void put_corridor_end(const struct Room *room, uint8_t x, uint8_t y)
{
    if (room->flags & ROOM_PHANTOM) {
        put_passage(x, y);
    } else {
        put_door(room, x, y);
    }
}

// folyosó a szoba és a jobb oldali vagy az alatta lévő szomszédja közt: a fő irányban halad,
// egy véletlen helyen oldalra fordul, amíg a célajtó vonalába nem ér, aztán megy tovább a fő irányban
static void dig_corridor(uint8_t from_index, uint8_t direction)
{
    const struct Room *from = &game.rooms[from_index];
    const struct Room *to = &game.rooms[get_neighbor_room(from_index, direction)];

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
        // ajtó a kiinduló szoba alsó falán és a célszoba felső falán, sarok nélkül
        if (!(from->flags & ROOM_PHANTOM)) {
            start_x = from->x + rnd(from->width - 2) + 1;
            start_y = from->y + from->height - 1;
        }
        if (!(to->flags & ROOM_PHANTOM)) {
            end_x = to->x + rnd(to->width - 2) + 1;
        }

        step_x = 0;
        step_y = 1;
        length = end_y - start_y - 1;
        turn_step_x = start_x < end_x ? 1 : -1;
        turn_step_y = 0;
        turn_length = start_x < end_x ? end_x - start_x : start_x - end_x;
    } else {
        // ajtó a kiinduló szoba jobb oldalfalán és a célszoba bal oldalfalán, sarok nélkül
        if (!(from->flags & ROOM_PHANTOM)) {
            start_x = from->x + from->width - 1;
            start_y = from->y + rnd(from->height - 2) + 1;
        }
        if (!(to->flags & ROOM_PHANTOM)) {
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

    put_corridor_end(from, start_x, start_y);
    put_corridor_end(to, end_x, end_y);

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

    x += step_x;
    y += step_y;
    DEV_CHECK(x == end_x && y == end_y, "corridor does not reach its end");
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

// ideiglenes: a folyosók után a szint-önteszt a térképen ellenőrzi, hogy minden szoba elérhető, akkor ez törlődik
#ifdef DEVMODE
bool are_rooms_connected(uint8_t room_a, uint8_t room_b)
{
    for (uint8_t direction = 0; direction < DIRECTION_COUNT; direction++) {
        if (get_neighbor_room(room_a, direction) == room_b) {
            return (connected_directions[room_a] & (1 << direction)) != 0;
        }
    }
    return false;
}
#endif
