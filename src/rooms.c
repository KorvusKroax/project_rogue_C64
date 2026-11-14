#include "rooms.h"

#include "corridors.h"
#include "map.h"
#include "rng.h"
#include "state.h"

#define PHANTOM_EDGE_DISTANCE  1                // a fantom szoba pontja ennyi mezőre van legalább a szektor szélétől (default: 1)

// a pont a szektor utolsó oszlopa és sora előtt: a folyosó hossza így legalább 1, és a legalsó szektorban sem kerül a státuszsorba
#if PHANTOM_EDGE_DISTANCE < 1
    #error "a PHANTOM_EDGE_DISTANCE legalább 1 kell legyen"
#endif

// a felső szektorsorban sem kerülhet a térkép fölé, az üzenetsorba
#if PHANTOM_EDGE_DISTANCE < MAP_TOP_ROW
    #error "a PHANTOM_EDGE_DISTANCE legalább MAP_TOP_ROW kell legyen"
#endif

// a két szél között marad hely a sorsoláshoz, különben a rnd tartománya 0 vagy negatív, és a pont kiesik a szektorból
#if 2 * PHANTOM_EDGE_DISTANCE >= SECTOR_WIDTH || 2 * PHANTOM_EDGE_DISTANCE >= SECTOR_HEIGHT
    #error "a PHANTOM_EDGE_DISTANCE túl nagy a szektorhoz"
#endif

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

// a labirintus cellái a páros relatív koordinátákon vannak, köztük egy-egy mező; a legnagyobb labirintus a szektor az utolsó oszlopa és sora nélkül
#define MAZE_MAX_CELLS_X  ((SECTOR_WIDTH - 1) / 2 + 1)                   // default: 13
#define MAZE_MAX_CELLS_Y  ((SECTOR_HEIGHT - 1) / 2 + 1)                  // default: 4
#define MAZE_STACK_SIZE   (MAZE_MAX_CELLS_X * MAZE_MAX_CELLS_Y)          // minden cella legfeljebb egyszer kerül a verembe (default: 52)

// csak az utolsó szektoroszlop és -sor labirintusa lóghat ki a térképről: a legtávolabbi cellája a helye + a mérete páros része;
// páratlan szektorszélességnél (pl. 81 oszlop) vagy 1×1-es rácsnál kell a méretet a térkép széléig csökkenteni, az alapértékekkel soha
#if SECTOR_GRID_SIZE == 1
    #define MAZE_LAST_X  0                                                                        // a bal szélső a 0. oszloptól kezdődik
#else
    #define MAZE_LAST_X  ((SECTOR_GRID_SIZE - 1) * SECTOR_WIDTH + SECTOR_FIRST_COLUMN)            // default: 53
#endif
#define MAZE_NEEDS_WIDTH_LIMIT  (MAZE_LAST_X + (SECTOR_WIDTH - 1) / 2 * 2 > MAP_COLUMNS - 1)      // default: 53 + 24 > 79 hamis

#define MAZE_LAST_CORNER_Y  ((SECTOR_GRID_SIZE - 1) * SECTOR_HEIGHT)                              // default: 16
#if MAZE_LAST_CORNER_Y < MAP_TOP_ROW
    #define MAZE_LAST_Y_REACH  (MAP_TOP_ROW + (SECTOR_HEIGHT - 1 - (MAP_TOP_ROW - MAZE_LAST_CORNER_Y)) / 2 * 2)
#else
    #define MAZE_LAST_Y_REACH  (MAZE_LAST_CORNER_Y + (SECTOR_HEIGHT - 1) / 2 * 2)                 // default: 16 + 6 = 22
#endif
#define MAZE_NEEDS_HEIGHT_LIMIT  (MAZE_LAST_Y_REACH > MAP_TOP_ROW + MAP_ROWS - 1)                 // default: 22 > 22 hamis

// 1×1-es rácsnál a verem 255-nél nagyobb is lehet
#if MAZE_STACK_SIZE > 255
    typedef uint16_t maze_stack_index_t;
#else
    typedef uint8_t maze_stack_index_t;
#endif

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

        // a legtávolabbi cella a méret páros része: ha a térképen kívül esne, a méret a térkép széléig csökken
        #if MAZE_NEEDS_WIDTH_LIMIT
            if (room->x + room->width / 2 * 2 > MAP_COLUMNS - 1) {
                room->width = MAP_COLUMNS - 1 - room->x;
            }
        #endif
        #if MAZE_NEEDS_HEIGHT_LIMIT
            if (room->y + room->height / 2 * 2 > MAP_TOP_ROW + MAP_ROWS - 1) {
                room->height = MAP_TOP_ROW + MAP_ROWS - 1 - room->y;
            }
        #endif
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

struct MazeCell {
    uint8_t x;                          // a labirintus bal felső sarkához képest
    uint8_t y;
};

static struct MazeCell maze_stack[MAZE_STACK_SIZE];

// a lépések sorrendje: x + 2, x - 2, y + 2, y - 2
static const int8_t maze_step_x[4] = { 2, -2, 0, 0 };
static const int8_t maze_step_y[4] = { 0, 0, 2, -2 };

// visszalépéses (DFS) labirintus folyosó-mezőkből; saját veremmel, mert a hardveres verem kicsi
static void draw_maze(const struct Room *room)
{
    uint8_t start_y = rnd(room->height) / 2 * 2;    // előbb az y, aztán az x
    uint8_t start_x = rnd(room->width) / 2 * 2;
    put_passage(room->x + start_x, room->y + start_y);

    maze_stack_index_t stack_count = 1;
    maze_stack[0].x = start_x;
    maze_stack[0].y = start_y;

    while (stack_count > 0) {
        struct MazeCell *cell = &maze_stack[stack_count - 1];
        uint8_t options = 0;                        // ennyi ásható szomszéd van
        uint8_t option_x[4];
        uint8_t option_y[4];

        for (uint8_t step = 0; step < 4; step++) {
            int8_t next_x = cell->x + maze_step_x[step];
            int8_t next_y = cell->y + maze_step_y[step];

            if (next_x < 0 || next_x > room->width || next_y < 0 || next_y > room->height) continue;
            if (get_map_flags(room->x + next_x, room->y + next_y) & TILE_PASSAGE) continue;

            option_x[options] = next_x;
            option_y[options] = next_y;
            options++;
        }

        // zsákutca: vissza az előző cellához
        if (options == 0) {
            stack_count--;
            continue;
        }

        // egyenletes választás az ásható szomszédok közül, egyetlen dobással; egy szomszédnál nincs mit dobni
        uint8_t chosen = 0;
        if (options > 1) {
            chosen = rnd(options);
        }
        uint8_t chosen_x = option_x[chosen];
        uint8_t chosen_y = option_y[chosen];

        // a két cella közti mező, aztán az új cella
        put_passage(room->x + (cell->x + chosen_x) / 2, room->y + (cell->y + chosen_y) / 2);
        put_passage(room->x + chosen_x, room->y + chosen_y);

        // nem csordul túl: minden cella legfeljebb egyszer kerül a verembe (MAZE_STACK_SIZE)
        maze_stack[stack_count].x = chosen_x;
        maze_stack[stack_count].y = chosen_y;
        stack_count++;
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

// a labirintus a legtávolabbi celláig ér: a helye + a mérete páros része, ami a felső szektorsorban a méretnél eggyel több is lehet
uint8_t get_room_at(uint8_t x, uint8_t y)
{
    bool is_passage = (get_map_flags(x, y) & TILE_PASSAGE) != 0;

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        const struct Room *room = &game.rooms[room_index];
        if (room->flags & ROOM_PHANTOM) continue;

        bool is_maze = (room->flags & ROOM_MAZE) != 0;
        if (is_passage && !is_maze) continue;

        uint8_t last_x = room->x + room->width - 1;
        uint8_t last_y = room->y + room->height - 1;
        if (is_maze) {
            last_x = room->x + room->width / 2 * 2;
            last_y = room->y + room->height / 2 * 2;
        }

        if (x >= room->x && x <= last_x && y >= room->y && y <= last_y) return room_index;
    }

    return IN_PASSAGE;
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

        if (room->flags & ROOM_MAZE) {
            draw_maze(room);
        } else {
            draw_room(room);
        }
    }
}
