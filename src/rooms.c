#include "rooms.h"

#include "dev/dev.h"
#include "map.h"
#include "rng.h"
#include "state.h"

// az i. szektor sora i / SECTOR_GRID_SIZE, oszlopa i % SECTOR_GRID_SIZE
#define SECTOR_WIDTH         (MAP_COLUMNS / SECTOR_GRID_SIZE)            // az egész osztás lefelé kerekít (default: 80 / 3 = 26)
#define SECTOR_HEIGHT        (LOGICAL_SCREEN_ROWS / SECTOR_GRID_SIZE)    // default: 24 / 3 = 8
#define SECTOR_FIRST_COLUMN  1                                           // a szektorok az 1. oszloptól kezdődnek (default: 1)

#define PHANTOM_EDGE_DISTANCE  1                // a fantom szoba pontja ennyi mezőre van legalább a szektor szélétől (default: 1)

#define PHANTOM_ROOM_LIMIT  3                   // egy szinten legfeljebb ennyi fantom szoba lehet (default: 3)

// legalább egy igazi szoba kell, különben a choose_random_room végtelen ciklusba fut
#if ROOM_COUNT - 1 < PHANTOM_ROOM_LIMIT
    #define MAX_PHANTOM_ROOMS  (ROOM_COUNT - 1)                  // default: 3
#else
    #define MAX_PHANTOM_ROOMS  PHANTOM_ROOM_LIMIT                // default: 3
#endif

#define MIN_ROOM_SIZE  4                        // falakkal együtt (default: 4)

// a szobának kell legalább egy belső mező, és a falain sarok nélküli hely az ajtónak
#if MIN_ROOM_SIZE < 3
    #error "a MIN_ROOM_SIZE legalább 3 kell legyen"
#endif

// a szektor utolsó oszlopa üres marad: legalább 1 oszlop a legkisebb szoba mellett;
// a felső szektorsorban a szoba nem kezdődhet a térkép fölött: a legkisebb szoba mellett MAP_TOP_ROW + 1 sor kell, különben az újrasorsolás végtelen
#if SECTOR_WIDTH < MIN_ROOM_SIZE + 1 || SECTOR_HEIGHT < MIN_ROOM_SIZE + MAP_TOP_ROW + 1
    #error "a szektor túl kicsi a legkisebb szobához"
#endif

#define DARK_ROOM_CHANCE_DIVISOR  10            // a szoba (mélység - 1) / DARK_ROOM_CHANCE_DIVISOR eséllyel sötét (default: 10)
#define MAZE_CHANCE_DIVISOR       15            // a sötét szobából 1 / MAZE_CHANCE_DIVISOR eséllyel labirintus lesz (default: 15)

// a szoba helye és mérete a szektorán belül, a flagjei szerint
static void place_room(uint8_t room_index)
{
    struct Room *room = &game.rooms[room_index];
    uint8_t corner_x = room_index % SECTOR_GRID_SIZE * SECTOR_WIDTH + SECTOR_FIRST_COLUMN;  // a szektor bal felső sarka
    uint8_t corner_y = room_index / SECTOR_GRID_SIZE * SECTOR_HEIGHT;

    // fantom szoba: egyetlen folyosó-csomópont, a szektor szélétől legalább PHANTOM_EDGE_DISTANCE mezőre
    if (room->flags & ROOM_PHANTOM) {
        room->x = corner_x + rnd(SECTOR_WIDTH - 2 * PHANTOM_EDGE_DISTANCE) + PHANTOM_EDGE_DISTANCE;
        room->y = corner_y + rnd(SECTOR_HEIGHT - 2 * PHANTOM_EDGE_DISTANCE) + PHANTOM_EDGE_DISTANCE;
        room->width = 0;
        room->height = 0;

        DEV_CHECK(room->y >= MAP_TOP_ROW && room->y < MAP_TOP_ROW + MAP_ROWS, "phantom room outside the map");
        return;
    }

    // labirintus szoba: kitölti a szektort, az utolsó oszlopa és sora nélkül; a bal szélső a 0. oszloptól kezdődik
    if (room->flags & ROOM_MAZE) {
        room->x = corner_x == SECTOR_FIRST_COLUMN ? 0 : corner_x;
        room->y = corner_y;
        room->width = SECTOR_WIDTH - 1;
        room->height = SECTOR_HEIGHT - 1;

        // a felső szektorsorban a térkép fölötti sorok kimaradnak
        if (room->y < MAP_TOP_ROW) {
            room->height -= MAP_TOP_ROW - room->y;
            room->y = MAP_TOP_ROW;
        }
        return;
    }

    // normál szoba: véletlen méret, és olyan hely, hogy a szektoron belül maradjon; ha a felső fala a térkép fölé esne, újrasorsol
    do {
        room->width = rnd(SECTOR_WIDTH - MIN_ROOM_SIZE) + MIN_ROOM_SIZE;
        room->height = rnd(SECTOR_HEIGHT - MIN_ROOM_SIZE) + MIN_ROOM_SIZE;
        room->x = corner_x + rnd(SECTOR_WIDTH - room->width);
        room->y = corner_y + rnd(SECTOR_HEIGHT - room->height);
    } while (room->y < MAP_TOP_ROW);
}

// normál szoba a MAP-ba: a felső és az alsó fal sarkokkal együtt vízszintes, az oldalfalak függőlegesek, belül padló
static void draw_room(const struct Room *room)
{
    uint8_t right_x = room->x + room->width - 1;
    uint8_t bottom_y = room->y + room->height - 1;

    DEV_CHECK(right_x < MAP_COLUMNS && bottom_y < MAP_TOP_ROW + MAP_ROWS, "room outside the map");

    for (uint8_t x = room->x; x <= right_x; x++) {
        set_map_character(x, room->y, CHAR_WALL_HORIZONTAL);
        set_map_character(x, bottom_y, CHAR_WALL_HORIZONTAL);
    }

    for (uint8_t y = room->y + 1; y < bottom_y; y++) {
        set_map_character(room->x, y, CHAR_WALL_VERTICAL);
        for (uint8_t x = room->x + 1; x < right_x; x++) {
            set_map_character(x, y, CHAR_FLOOR);
        }
        set_map_character(right_x, y, CHAR_WALL_VERTICAL);
    }
}

uint8_t choose_random_room(void)
{
    uint8_t room_index = rnd(ROOM_COUNT);

    while (game.rooms[room_index].flags & ROOM_PHANTOM) {
        room_index = rnd(ROOM_COUNT);
    }

    return room_index;
}

void generate_rooms(void)
{
    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        game.rooms[room_index].flags = 0;
    }

    // a choose_random_room csak olyan szobát választ, amelyik még nem fantom
    uint8_t phantom_room_count = rnd(MAX_PHANTOM_ROOMS + 1);
    for (uint8_t i = 0; i < phantom_room_count; i++) {
        uint8_t phantom_room_index = choose_random_room();      // külön változóban: tömbindexként a |= -ben az Oscar64 -O2 elszáll
        game.rooms[phantom_room_index].flags |= ROOM_PHANTOM;
    }

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        struct Room *room = &game.rooms[room_index];

        if (room->flags & ROOM_PHANTOM) {
            place_room(room_index);
            continue;
        }

        // a mélységgel nő a sötét szobák aránya, az 1. szinten nincs ilyen
        if (rnd(DARK_ROOM_CHANCE_DIVISOR) < game.dungeon_level - 1) {
            room->flags |= ROOM_DARK;

            // értékadás: a labirintus nem sötét
            if (rnd(MAZE_CHANCE_DIVISOR) == 0) {
                room->flags = ROOM_MAZE;
            }
        }

        place_room(room_index);

        // a labirintus rajzolása a következő lépésben jön
        if (!(room->flags & ROOM_MAZE)) {
            draw_room(room);
        }
    }
}
