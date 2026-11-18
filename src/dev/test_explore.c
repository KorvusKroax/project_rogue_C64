#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../explore.h"
#include "../map.h"
#include "../monsters.h"
#include "../move.h"
#include "../state.h"
#include "../view.h"

// az esetek a térkép közepén, a BASE_X, BASE_Y mezőtől jobbra lefelé épülnek
#define BASE_X  (MAP_COLUMNS / 2)
#define BASE_Y  (MAP_TOP_ROW + MAP_ROWS / 2)

static void put_test_tile(uint8_t offset_x, uint8_t offset_y, uint8_t character)
{
    set_map_character(BASE_X + offset_x, BASE_Y + offset_y, character);
}

// a tesztek folyosó-mezői mind ehhez a folyosórendszerhez tartoznak
#define TEST_PASSAGE_NUMBER  1

static void put_test_passage(uint8_t offset_x, uint8_t offset_y)
{
    put_test_tile(offset_x, offset_y, CHAR_PASSAGE);
    add_map_flags(BASE_X + offset_x, BASE_Y + offset_y, TILE_PASSAGE | TEST_PASSAGE_NUMBER);
}

static bool is_test_step_allowed(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y)
{
    return is_diagonal_step_allowed(BASE_X + from_x, BASE_Y + from_y, BASE_X + to_x, BASE_Y + to_y);
}

static bool is_test_neighbor_visible(uint8_t from_x, uint8_t from_y, uint8_t x, uint8_t y)
{
    return is_neighbor_visible(BASE_X + from_x, BASE_Y + from_y, BASE_X + x, BASE_Y + y);
}

static bool is_test_interesting(int8_t step_x, int8_t step_y)
{
    // a játékos a (2, 2) mezőn áll, hogy mind a nyolc szomszéd pozitív eltolású legyen
    return is_something_interesting_beside(BASE_X + 2, BASE_Y + 2, step_x, step_y);
}

// a (2, 2) mezőről step_x, step_y irányba futva befordul-e, és ha igen, az új irány expected_x, expected_y-e
static bool is_test_turn(int8_t step_x, int8_t step_y, int8_t expected_x, int8_t expected_y)
{
    if (!find_passage_turn(BASE_X + 2, BASE_Y + 2, &step_x, &step_y)) return false;
    return step_x == expected_x && step_y == expected_y;
}

static bool is_test_turn_found(int8_t step_x, int8_t step_y)
{
    return find_passage_turn(BASE_X + 2, BASE_Y + 2, &step_x, &step_y);
}

// a (0, 0)..(4, 4) terület padló, a játékos (2, 2) mezője körül
static void put_test_floor(void)
{
    clear_map();
    for (uint8_t y = 0; y < 5; y++) {
        for (uint8_t x = 0; x < 5; x++) {
            put_test_tile(x, y, CHAR_FLOOR);
        }
    }
}

int test_diagonal_step(uint16_t i)
{
    // folyosókanyar: átlósan nem lehet átvágni, ha a sarok egyik mezője kő
    clear_map();
    put_test_passage(0, 0);
    put_test_passage(1, 0);
    put_test_passage(1, 1);
    if (is_test_step_allowed(0, 0, 1, 1)) return false;
    if (!is_test_step_allowed(0, 0, 1, 0)) return false;

    // ajtóra és ajtóról átlósan nem, mert mellette fal van
    clear_map();
    put_test_passage(0, 0);
    put_test_passage(0, 1);
    put_test_tile(1, 0, CHAR_WALL_VERTICAL);
    put_test_tile(1, 1, CHAR_DOOR);
    put_test_tile(1, 2, CHAR_WALL_VERTICAL);
    put_test_tile(2, 0, CHAR_FLOOR);
    put_test_tile(2, 1, CHAR_FLOOR);
    if (is_test_step_allowed(0, 0, 1, 1)) return false;
    if (is_test_step_allowed(1, 1, 2, 0)) return false;
    if (!is_test_step_allowed(0, 1, 1, 1)) return false;

    // szobában átlósan is
    clear_map();
    put_test_tile(0, 0, CHAR_FLOOR);
    put_test_tile(1, 0, CHAR_FLOOR);
    put_test_tile(0, 1, CHAR_FLOOR);
    put_test_tile(1, 1, CHAR_STAIRS);
    if (!is_test_step_allowed(0, 0, 1, 1)) return false;

    clear_map();
    return true;
}

int test_neighbor_visibility(uint16_t i)
{
    // két folyosó-mező átlósan, a sarok mindkét mezője kő: nem látszik
    clear_map();
    put_test_passage(0, 0);
    put_test_passage(1, 1);
    if (is_test_neighbor_visible(0, 0, 1, 1)) return false;

    // folyosókanyar: a sarok egyik mezője folyosó, átlósan is látszik
    put_test_passage(1, 0);
    if (!is_test_neighbor_visible(0, 0, 1, 1)) return false;

    // a titkos folyosó-mező (kőzet jellel) nem látszik
    clear_map();
    put_test_passage(0, 0);
    put_test_tile(1, 0, CHAR_SOLID_ROCK);
    add_map_flags(BASE_X + 1, BASE_Y, TILE_PASSAGE);
    remove_map_flags(BASE_X + 1, BASE_Y, TILE_REAL);
    if (is_test_neighbor_visible(0, 0, 1, 0)) return false;

    // folyosóról a szoba fala nem látszik, az ajtó igen, és az ajtóról a folyosó és a szoba is
    clear_map();
    put_test_passage(0, 0);
    put_test_passage(0, 1);
    put_test_tile(1, 0, CHAR_WALL_VERTICAL);
    put_test_tile(1, 1, CHAR_DOOR);
    put_test_tile(2, 0, CHAR_FLOOR);
    put_test_tile(2, 1, CHAR_FLOOR);
    if (is_test_neighbor_visible(0, 0, 1, 0)) return false;
    if (!is_test_neighbor_visible(0, 0, 1, 1)) return false;
    if (!is_test_neighbor_visible(1, 1, 0, 0)) return false;
    if (!is_test_neighbor_visible(1, 1, 2, 0)) return false;
    if (!is_test_neighbor_visible(1, 1, 1, 0)) return false;

    // szobából a fal és az ajtó is látszik
    if (!is_test_neighbor_visible(2, 0, 1, 0)) return false;
    if (!is_test_neighbor_visible(2, 1, 1, 1)) return false;

    // szoba- és folyosó-mező ajtó nélkül egymás mellett: egyikről sem látszik a másik
    clear_map();
    put_test_tile(0, 0, CHAR_FLOOR);
    put_test_passage(1, 0);
    if (is_test_neighbor_visible(0, 0, 1, 0)) return false;
    if (is_test_neighbor_visible(1, 0, 0, 0)) return false;

    // szobában átlósan is látszik
    clear_map();
    put_test_tile(0, 0, CHAR_FLOOR);
    put_test_tile(1, 1, CHAR_FLOOR);
    if (!is_test_neighbor_visible(0, 0, 1, 1)) return false;

    clear_map();
    return true;
}

int test_careful_run(uint16_t i)
{
    // egyenes folyosó: csak az elülső folyosó-mező számít, a hátsó nem
    clear_map();
    put_test_passage(1, 2);
    put_test_passage(2, 2);
    put_test_passage(3, 2);
    if (is_test_interesting(1, 0)) return false;

    // elágazás: elöl és oldalt is folyosó
    put_test_passage(2, 1);
    if (!is_test_interesting(1, 0)) return false;

    // szobában a fal mellett: az oldalt lévő ajtónál megáll, az átlósnál nem
    put_test_floor();
    put_test_tile(1, 1, CHAR_WALL_HORIZONTAL);
    put_test_tile(2, 1, CHAR_WALL_HORIZONTAL);
    put_test_tile(3, 1, CHAR_DOOR);
    if (is_test_interesting(1, 0)) return false;
    put_test_tile(2, 1, CHAR_DOOR);
    put_test_tile(3, 1, CHAR_WALL_HORIZONTAL);
    if (!is_test_interesting(1, 0)) return false;

    // a lépcső átlósan elöl számít, hátul nem
    put_test_floor();
    put_test_tile(3, 3, CHAR_STAIRS);
    if (!is_test_interesting(1, 0)) return false;
    if (is_test_interesting(-1, 0)) return false;

    // átlós futásnál a hátsó félsík három mezője kimarad, az oldalsó átlók nem
    put_test_floor();
    put_test_tile(1, 2, CHAR_STAIRS);
    if (is_test_interesting(1, 1)) return false;
    put_test_tile(1, 2, CHAR_FLOOR);
    put_test_tile(1, 3, CHAR_STAIRS);
    if (!is_test_interesting(1, 1)) return false;

    clear_map();
    return true;
}

int test_passage_turn(uint16_t i)
{
    // kanyar: jobbra futva lefelé, felfelé futva balra fordul
    clear_map();
    put_test_passage(1, 2);
    put_test_passage(2, 2);
    put_test_passage(2, 3);
    if (!is_test_turn(1, 0, 0, 1)) return false;
    if (!is_test_turn(0, -1, -1, 0)) return false;

    // egyenes folyosóban, ahol továbbmehet, nem fordul; átlósan sem
    if (is_test_turn_found(-1, 0)) return false;
    if (is_test_turn_found(1, 1)) return false;

    // elágazás: mindkét oldalon folyosó
    put_test_passage(2, 1);
    if (is_test_turn_found(1, 0)) return false;

    // zsákutca: egyik oldalon sincs semmi
    clear_map();
    put_test_passage(1, 2);
    put_test_passage(2, 2);
    if (is_test_turn_found(1, 0)) return false;

    // a rejtett folyosó-mező felé nem fordul
    put_test_passage(2, 3);
    remove_map_flags(BASE_X + 2, BASE_Y + 3, TILE_REAL);
    if (is_test_turn_found(1, 0)) return false;

    // ajtó felé igen
    clear_map();
    put_test_passage(1, 2);
    put_test_passage(2, 2);
    put_test_tile(2, 1, CHAR_DOOR);
    if (!is_test_turn(1, 0, 0, -1)) return false;

    // szobában (nem F_PASS mezőn) nem fordul
    put_test_floor();
    put_test_tile(3, 2, CHAR_WALL_VERTICAL);
    put_test_tile(2, 1, CHAR_WALL_HORIZONTAL);
    if (is_test_turn_found(1, 0)) return false;

    clear_map();
    return true;
}

// a tesztszoba: a 0. szoba a BASE_X, BASE_Y sarokkal, falakkal együtt 9 × 5; a bal falán, a 2. sorban ajtó, előtte folyosó-mező
#define SEE_ROOM_WIDTH   9
#define SEE_ROOM_HEIGHT  5
#define SEE_DOOR_Y       2

// a többi szoba fantom, hogy a get_room_at csak a tesztszobát találja; a folyosó-mezők folyosórendszere sötét, mint a generált szinten
static void put_see_test_room(uint8_t flags)
{
    clear_map();
    clear_monsters();
    clear_seen_map();

    for (uint8_t room_index = 0; room_index < ROOM_COUNT; room_index++) {
        game.rooms[room_index].flags = ROOM_PHANTOM;
    }
    game.rooms[PASSAGE_ROOM_INDEX(TEST_PASSAGE_NUMBER)].flags = ROOM_PHANTOM | ROOM_DARK;
    struct Room *room = &game.rooms[0];
    room->x = BASE_X;
    room->y = BASE_Y;
    room->width = SEE_ROOM_WIDTH;
    room->height = SEE_ROOM_HEIGHT;
    room->flags = flags;

    for (uint8_t y = 0; y < SEE_ROOM_HEIGHT; y++) {
        for (uint8_t x = 0; x < SEE_ROOM_WIDTH; x++) {
            uint8_t character = CHAR_FLOOR;
            if (y == 0 || y == SEE_ROOM_HEIGHT - 1) {
                character = CHAR_WALL_HORIZONTAL;
            } else if (x == 0 || x == SEE_ROOM_WIDTH - 1) {
                character = CHAR_WALL_VERTICAL;
            }
            put_test_tile(x, y, character);
        }
    }
    put_test_tile(0, SEE_DOOR_Y, CHAR_DOOR);
    set_map_character(BASE_X - 1, BASE_Y + SEE_DOOR_Y, CHAR_PASSAGE);
    add_map_flags(BASE_X - 1, BASE_Y + SEE_DOOR_Y, TILE_PASSAGE | TEST_PASSAGE_NUMBER);

    game.player.x = BASE_X + 2;
    game.player.y = BASE_Y + 2;
    game.player.room = 0;
}

static struct Monster *put_see_test_monster(uint8_t type, uint8_t offset_x, uint8_t offset_y)
{
    return get_monster(create_monster(type, BASE_X + offset_x, BASE_Y + offset_y));
}

static uint8_t get_test_seen_character(uint8_t offset_x, uint8_t offset_y)
{
    return get_seen_character(BASE_X + offset_x, BASE_Y + offset_y);
}

// világos szoba: a szoba minden szörnye látszik, a phantom nem; belépéskor kirajzolódnak, kilépéskor a helyükre padló (ajtón ajtó) kerül
static bool is_lit_room_drawing_valid(void)
{
    put_see_test_room(0);
    struct Monster *bat = put_see_test_monster('B', 6, 2);
    struct Monster *phantom = put_see_test_monster('P', 6, 3);
    put_see_test_monster('K', 0, SEE_DOOR_Y);

    if (!is_monster_visible(bat) || is_monster_visible(phantom)) return false;

    enter_room(game.player.x, game.player.y);
    if (get_test_seen_character(6, 2) != 'B') return false;
    if (get_test_seen_character(6, 3) != CHAR_FLOOR) return false;
    if (get_test_seen_character(0, SEE_DOOR_Y) != 'K') return false;
    if (bat->character_below != CHAR_FLOOR) return false;

    leave_room(BASE_X - 1, BASE_Y + SEE_DOOR_Y);
    if (game.player.room != PASSAGE_ROOM_INDEX(TEST_PASSAGE_NUMBER)) return false;
    if (get_test_seen_character(6, 2) != CHAR_FLOOR) return false;
    return get_test_seen_character(0, SEE_DOOR_Y) == CHAR_DOOR;
}

// sötét szoba: csak a szomszédos szörny látszik, átlósan sarkon át nem; kilépéskor a jele helyére üres mező kerül
static bool is_dark_room_drawing_valid(void)
{
    put_see_test_room(ROOM_DARK);
    struct Monster *bat = put_see_test_monster('B', 6, 2);
    struct Monster *emu = put_see_test_monster('E', 3, 2);
    struct Monster *snake = put_see_test_monster('S', 3, 3);

    if (is_monster_visible(bat) || !is_monster_visible(emu) || !is_monster_visible(snake)) return false;

    look_around();
    if (get_test_seen_character(3, 2) != 'E') return false;
    if (get_test_seen_character(6, 2) != CHAR_SOLID_ROCK) return false;

    leave_room(BASE_X - 1, BASE_Y + SEE_DOOR_Y);
    if (get_test_seen_character(3, 2) != CHAR_SOLID_ROCK) return false;

    // a sarok mindkét mezője fal: átlósan nem látszik
    put_test_tile(2, 3, CHAR_WALL_HORIZONTAL);
    put_test_tile(3, 2, CHAR_WALL_VERTICAL);
    game.player.room = 0;
    return !is_monster_visible(snake);
}

// folyosón csak a szomszédos szörny látszik
static bool is_passage_drawing_valid(void)
{
    put_see_test_room(0);
    put_test_passage(0, 6);
    put_test_passage(1, 6);
    put_test_passage(2, 6);
    game.player.x = BASE_X;
    game.player.y = BASE_Y + 6;
    game.player.room = PASSAGE_ROOM_INDEX(TEST_PASSAGE_NUMBER);

    struct Monster *near = put_see_test_monster('H', 1, 6);
    struct Monster *far = put_see_test_monster('Z', 2, 6);
    return is_monster_visible(near) && !is_monster_visible(far);
}

int test_monster_visibility(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_lit_room_drawing_valid() &&
        is_dark_room_drawing_valid() &&
        is_passage_drawing_valid();

    memcpy(game.rooms, saved_rooms, sizeof(saved_rooms));
    clear_monsters();
    clear_map();
    clear_seen_map();
    return ok;
}

#define WAKE_TEST_SEED       12345
#define WAKE_TEST_ROUNDS     150                // ennyiszer lép be a játékos az aljas szörny szobájába
#define WAKE_TEST_EXPECTED   (WAKE_TEST_ROUNDS * 2 / 3)
#define WAKE_TEST_TOLERANCE  (WAKE_TEST_ROUNDS / 10)

#if WAKE_TEST_ROUNDS > 255
    #error "a WAKE_TEST_ROUNDS számlálója 8 bites"
#endif

// az aljas szörny (hobgoblin) a szobájába lépve kb. 2/3 eséllyel ébred, rögzített maggal; sötét szobában is
static bool is_mean_monster_waking_valid(void)
{
    put_see_test_room(ROOM_DARK);
    struct Monster *hobgoblin = put_see_test_monster('H', 6, 2);
    game.seed = WAKE_TEST_SEED;

    uint8_t woken_count = 0;
    for (uint8_t round = 0; round < WAKE_TEST_ROUNDS; round++) {
        hobgoblin->flags &= ~MONSTER_RUNNING;
        enter_room(game.player.x, game.player.y);
        if (hobgoblin->flags & MONSTER_RUNNING) {
            woken_count++;
        }
    }
    return woken_count >= WAKE_TEST_EXPECTED - WAKE_TEST_TOLERANCE && woken_count <= WAKE_TEST_EXPECTED + WAKE_TEST_TOLERANCE;
}

// a nem aljas, nem kapzsi szörny (kentaur), és a fogva tartott aljas sem ébred, sem belépéskor, sem a játékos mellett
static bool is_sleeping_monster_waking_valid(void)
{
    put_see_test_room(ROOM_DARK);
    struct Monster *centaur = put_see_test_monster('C', 3, 2);
    struct Monster *hobgoblin = put_see_test_monster('H', 2, 3);
    hobgoblin->flags |= MONSTER_HELD;

    for (uint8_t round = 0; round < WAKE_TEST_ROUNDS; round++) {
        enter_room(game.player.x, game.player.y);
        look_around();
    }
    return !(centaur->flags & MONSTER_RUNNING) && !(hobgoblin->flags & MONSTER_RUNNING);
}

// a kapzsi szörny (ork) mindig ébred: belépéskor, kilépéskor és a játékos mellett, a célpontja a játékos;
// a look_around csak a szomszédosat ébreszti
static bool is_greedy_monster_waking_valid(void)
{
    put_see_test_room(0);
    struct Monster *orc = put_see_test_monster('O', 6, 2);
    orc->destination_item = 1;

    enter_room(game.player.x, game.player.y);
    if (!(orc->flags & MONSTER_RUNNING) || orc->destination_item != DESTINATION_PLAYER) return false;

    orc->flags &= ~MONSTER_RUNNING;
    leave_room(BASE_X - 1, BASE_Y + SEE_DOOR_Y);
    if (!(orc->flags & MONSTER_RUNNING)) return false;

    orc->flags &= ~MONSTER_RUNNING;
    game.player.room = 0;
    struct Monster *neighbor_orc = put_see_test_monster('O', 3, 3);
    look_around();
    return !(orc->flags & MONSTER_RUNNING) && (neighbor_orc->flags & MONSTER_RUNNING);
}

// folyosón (új szint kezdetén) az enter_room a folyosórendszer szörnyeit nem ébreszti
static bool is_passage_entry_waking_valid(void)
{
    put_see_test_room(0);
    uint8_t passage_x = BASE_X - 2;
    uint8_t passage_y = BASE_Y + SEE_DOOR_Y;
    set_map_character(passage_x, passage_y, CHAR_PASSAGE);
    add_map_flags(passage_x, passage_y, TILE_PASSAGE | TEST_PASSAGE_NUMBER);
    struct Monster *orc = get_monster(create_monster('O', passage_x, passage_y));

    game.player.x = BASE_X - 1;
    enter_room(game.player.x, game.player.y);
    return !(orc->flags & MONSTER_RUNNING);
}

int test_monster_waking(uint16_t i)
{
    struct Room saved_rooms[ROOM_TABLE_SIZE];
    memcpy(saved_rooms, game.rooms, sizeof(saved_rooms));

    bool ok =
        is_mean_monster_waking_valid() &&
        is_sleeping_monster_waking_valid() &&
        is_greedy_monster_waking_valid() &&
        is_passage_entry_waking_valid();

    memcpy(game.rooms, saved_rooms, sizeof(saved_rooms));
    clear_monsters();
    clear_map();
    clear_seen_map();
    return ok;
}

// a rejtett mező a próbálkozások során előkerül; legfeljebb ennyi próbát vár (az esély 1/5 vagy 1/3)
#define SEARCH_TRIES_MAX  200

static bool is_test_tile_found(uint8_t offset_x, uint8_t offset_y)
{
    for (uint8_t tries = 0; tries < SEARCH_TRIES_MAX; tries++) {
        if (try_to_find_hidden_tile(BASE_X + offset_x, BASE_Y + offset_y)) return true;
    }
    return false;
}

int test_search(uint16_t i)
{
    // titkos ajtó: a fal jeléből ajtó lesz, és látható
    clear_map();
    put_test_tile(0, 0, CHAR_WALL_VERTICAL);
    remove_map_flags(BASE_X, BASE_Y, TILE_REAL);
    if (!is_test_tile_found(0, 0)) return false;
    if (get_map_character(BASE_X, BASE_Y) != CHAR_DOOR) return false;
    if (!(get_map_flags(BASE_X, BASE_Y) & TILE_REAL)) return false;

    // titkos folyosó-mező: a kőzet jeléből folyosó lesz
    put_test_tile(1, 0, CHAR_SOLID_ROCK);
    add_map_flags(BASE_X + 1, BASE_Y, TILE_PASSAGE);
    remove_map_flags(BASE_X + 1, BASE_Y, TILE_REAL);
    if (!is_test_tile_found(1, 0)) return false;
    if (get_map_character(BASE_X + 1, BASE_Y) != CHAR_PASSAGE) return false;

    // a látható mező nem változik, és a már megtalált sem
    put_test_tile(2, 0, CHAR_WALL_VERTICAL);
    if (is_test_tile_found(2, 0)) return false;
    if (is_test_tile_found(0, 0)) return false;

    clear_map();
    return true;
}
