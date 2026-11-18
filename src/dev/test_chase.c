#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../chase.h"
#include "../map.h"
#include "../monsters.h"
#include "../rooms.h"
#include "../state.h"
#include "../view.h"

// a tesztpálya a CHASE_BASE_X, CHASE_BASE_Y mezőtől jobbra lefelé épül
#define CHASE_BASE_X  30
#define CHASE_BASE_Y  (MAP_TOP_ROW + 2)

// a tesztpálya két szobát, két folyosórendszert és három kijáratot használ
#if ROOM_COUNT < 2 || MAX_PASSAGES < 2 || MAX_EXITS < 3
    #error "a tesztpályához legalább 2 szoba, 2 folyosórendszer és 3 kijárat kell"
#endif

// normál szoba falakkal, padlóval; a szobatáblába is bekerül
static void put_chase_room(uint8_t room_index, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height)
{
    struct Room *room = &game.rooms[room_index];
    room->x = CHASE_BASE_X + offset_x;
    room->y = CHASE_BASE_Y + offset_y;
    room->width = width;
    room->height = height;
    room->flags = 0;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint8_t character = CHAR_FLOOR;
            if (y == 0 || y == height - 1) {
                character = CHAR_WALL_HORIZONTAL;
            } else if (x == 0 || x == width - 1) {
                character = CHAR_WALL_VERTICAL;
            }
            set_map_character(room->x + x, room->y + y, character);
        }
    }
}

// ajtó a szoba falán, a folyosórendszer sorszámával, és a kijárata az ajtótáblában
static void put_chase_door(uint8_t offset_x, uint8_t offset_y, uint8_t room_index, uint8_t passage_number)
{
    uint8_t x = CHASE_BASE_X + offset_x;
    uint8_t y = CHASE_BASE_Y + offset_y;
    set_map_character(x, y, CHAR_DOOR);
    add_map_flags(x, y, passage_number);

    struct Exit *exit = &game.exits[game.exit_count];
    exit->x = x;
    exit->y = y;
    exit->room = room_index;
    exit->passage_number = passage_number;
    game.exit_count++;
}

static void put_chase_passage(uint8_t offset_x, uint8_t offset_y, uint8_t passage_number)
{
    uint8_t x = CHASE_BASE_X + offset_x;
    uint8_t y = CHASE_BASE_Y + offset_y;
    set_map_character(x, y, CHAR_PASSAGE);
    add_map_flags(x, y, TILE_PASSAGE | passage_number);
}

// a tesztpálya (eltolásokkal):
//  - az 1. szoba: (0, 0)..(6, 4), az alsó falán ajtó a (3, 4) mezőn;
//  - a 0. szoba: (10, 6)..(21, 12), ajtó a bal falán a (10, 9), a jobb falán a (21, 9) mezőn;
//  - az 1. folyosórendszer az 1. szoba ajtajától lefelé a (3, 9) mezőig, onnan jobbra a 0. szoba bal ajtajáig;
//  - a 2. folyosórendszer a 0. szoba jobb ajtajától jobbra, a (24, 9) mezőig;
// a többi szoba fantom, hogy a get_room_at csak ezt a kettőt találja
static void put_chase_level(void)
{
    clear_map();
    clear_monsters();
    clear_seen_map();
    game.dungeon_level = 1;
    game.exit_count = 0;

    for (uint8_t room_index = 0; room_index < ROOM_TABLE_SIZE; room_index++) {
        game.rooms[room_index].flags = room_index < ROOM_COUNT ? ROOM_PHANTOM : ROOM_PHANTOM | ROOM_DARK;
    }
    put_chase_room(0, 10, 6, 12, 7);
    put_chase_room(1, 0, 0, 7, 5);

    put_chase_door(10, 9, 0, 1);
    put_chase_door(21, 9, 0, 2);
    put_chase_door(3, 4, 1, 1);

    for (uint8_t y = 5; y <= 9; y++) {
        put_chase_passage(3, y, 1);
    }
    for (uint8_t x = 4; x <= 9; x++) {
        put_chase_passage(x, 9, 1);
    }
    for (uint8_t x = 22; x <= 24; x++) {
        put_chase_passage(x, 9, 2);
    }
}

static void put_chase_player(uint8_t offset_x, uint8_t offset_y)
{
    game.player.x = CHASE_BASE_X + offset_x;
    game.player.y = CHASE_BASE_Y + offset_y;
    game.player.room = get_room_at(game.player.x, game.player.y);
}

// ébren lévő szörny, a célpontja a játékos
static struct Monster *put_chase_monster(uint8_t type, uint8_t offset_x, uint8_t offset_y)
{
    struct Monster *monster = get_monster(create_monster(type, CHASE_BASE_X + offset_x, CHASE_BASE_Y + offset_y));
    start_monster_running(monster);
    return monster;
}

static void run_monster_turns(uint8_t turn_count)
{
    for (uint8_t turn = 0; turn < turn_count; turn++) {
        move_monsters();
    }
}

static bool is_monster_at(const struct Monster *monster, uint8_t offset_x, uint8_t offset_y)
{
    return monster->x == CHASE_BASE_X + offset_x && monster->y == CHASE_BASE_Y + offset_y;
}

// ugyanabban a szobában a zombi egyenesen a játékos felé lép, mellé érve megáll, és nem lép rá
static bool is_same_room_chase_valid(void)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *zombie = put_chase_monster('Z', 20, 9);

    run_monster_turns(1);
    if (!is_monster_at(zombie, 19, 9)) return false;
    run_monster_turns(7);
    if (!is_monster_at(zombie, 12, 9)) return false;
    run_monster_turns(3);
    return is_monster_at(zombie, 12, 9);
}

// a 0. szobában a (20, 9) mezőről két kör alatt megtett lépések, ha a játékos a (11, 9) mezőn áll
static uint8_t count_steps_in_two_turns(uint8_t type, uint16_t added_flags)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *monster = put_chase_monster(type, 20, 9);
    monster->flags |= added_flags;

    run_monster_turns(2);
    return CHASE_BASE_X + 20 - monster->x;
}

// körönként egy lépés; a lassított minden második körben, a gyorsított és a távoli repülő (kestrel) kétszer; a Venus flytrap soha
static bool are_monster_speeds_valid(void)
{
    return count_steps_in_two_turns('Z', 0) == 2
        && count_steps_in_two_turns('Z', MONSTER_SLOW) == 1
        && count_steps_in_two_turns('Z', MONSTER_HASTED) == 4
        && count_steps_in_two_turns('K', 0) == 4
        && count_steps_in_two_turns('F', 0) == 0;
}

// a játékos az 1. szobában: a zombi nem egyenesen felé indul (a bal falnál elakadna), hanem a 0. szoba hozzá legközelebbi
// kijáratára, a bal ajtóra; az ajtón a folyosórendszer kijáratai közül az 1. szoba ajtaját választja, és a folyosón át
// a játékos mellé ér
static bool is_exit_chase_valid(void)
{
    put_chase_level();
    put_chase_player(3, 2);
    struct Monster *zombie = put_chase_monster('Z', 19, 7);

    run_monster_turns(9);
    if (!is_monster_at(zombie, 10, 9) || zombie->room != 0) return false;
    run_monster_turns(1);
    if (!is_monster_at(zombie, 9, 9) || zombie->room != PASSAGE_ROOM_INDEX(1)) return false;
    run_monster_turns(11);
    if (!is_monster_at(zombie, 3, 4) || zombie->room != 1) return false;
    run_monster_turns(1);
    if (!is_monster_at(zombie, 3, 3)) return false;
    run_monster_turns(2);
    return is_monster_at(zombie, 3, 3);
}

#define BAT_TEST_SEED       2468
#define BAT_TEST_TRIALS     100
// a (20, 9) mezőről a (11, 9) mezőn álló játékos felé az egyenes lépés a (19, 9): a lépések fele ez, a véletlen fél 1/9-e is,
// így 5/9 eséllyel; a véletlen fél 5/9-e a többi járható szomszéd (két-két padló fölötte és alatta, és a jobb ajtó), így 5/18;
// a többi esetben fal vagy a saját mező, ott helyben marad
#define BAT_TEST_STRAIGHT_EXPECTED  (BAT_TEST_TRIALS * 5 / 9)
#define BAT_TEST_SIDEWAYS_EXPECTED  (BAT_TEST_TRIALS * 5 / 18)
#define BAT_TEST_TOLERANCE          (BAT_TEST_TRIALS * 15 / 100)

#if BAT_TEST_TRIALS > 255
    #error "a BAT_TEST_TRIALS számlálója 8 bites"
#endif

// a szörny vissza a megadott mezőre, a TILE_MONSTER bittel együtt
static void put_monster_back(struct Monster *monster, uint8_t offset_x, uint8_t offset_y)
{
    remove_map_flags(monster->x, monster->y, TILE_MONSTER);
    monster->x = CHASE_BASE_X + offset_x;
    monster->y = CHASE_BASE_Y + offset_y;
    monster->room = get_room_at(monster->x, monster->y);
    add_map_flags(monster->x, monster->y, TILE_MONSTER);
}

// a denevér kaotikusan mozog: a lépései fele véletlen irányba megy; rögzített maggal, mindig ugyanarról a mezőről, repülés nélkül
static bool is_bat_movement_chaotic(void)
{
    put_chase_level();
    put_chase_player(11, 9);
    struct Monster *bat = put_chase_monster('B', 20, 9);
    bat->flags &= ~MONSTER_FLYING;
    game.seed = BAT_TEST_SEED;

    uint8_t straight_count = 0;
    uint8_t sideways_count = 0;
    for (uint8_t trial = 0; trial < BAT_TEST_TRIALS; trial++) {
        run_monster_turns(1);
        if (is_monster_at(bat, 19, 9)) {
            straight_count++;
        } else if (!is_monster_at(bat, 20, 9)) {
            sideways_count++;
        }
        put_monster_back(bat, 20, 9);
    }

    bool is_straight_count_valid = straight_count >= BAT_TEST_STRAIGHT_EXPECTED - BAT_TEST_TOLERANCE
                                && straight_count <= BAT_TEST_STRAIGHT_EXPECTED + BAT_TEST_TOLERANCE;
    bool is_sideways_count_valid = sideways_count >= BAT_TEST_SIDEWAYS_EXPECTED - BAT_TEST_TOLERANCE
                                && sideways_count <= BAT_TEST_SIDEWAYS_EXPECTED + BAT_TEST_TOLERANCE;
    return is_straight_count_valid && is_sideways_count_valid;
}

int test_monster_chase(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_same_room_chase_valid() &&
        are_monster_speeds_valid() &&
        is_exit_chase_valid() &&
        is_bat_movement_chaotic();

    memcpy(game.rooms, saved_rooms, sizeof(saved_rooms));
    game.exit_count = 0;
    clear_monsters();
    clear_map();
    clear_seen_map();
    return ok;
}
