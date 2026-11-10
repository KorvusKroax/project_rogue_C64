#include "corridors.h"

#include "rng.h"
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

// a szoba és az adott irányban lévő szomszédja összekötve
static void connect_neighbor_rooms(uint8_t room_index, uint8_t direction)
{
    uint8_t neighbor_index = get_neighbor_room(room_index, direction);

    connected_directions[room_index] |= 1 << direction;
    connected_directions[neighbor_index] |= 1 << OPPOSITE_DIRECTION(direction);
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
