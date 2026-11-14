#include "tests.h"

#include <stdbool.h>

#include "../explore.h"
#include "../map.h"
#include "../move.h"
#include "../state.h"

// az esetek a térkép közepén, a BASE_X, BASE_Y mezőtől jobbra lefelé épülnek
#define BASE_X  (MAP_COLUMNS / 2)
#define BASE_Y  (MAP_TOP_ROW + MAP_ROWS / 2)

static void put_test_tile(uint8_t offset_x, uint8_t offset_y, uint8_t character)
{
    set_map_character(BASE_X + offset_x, BASE_Y + offset_y, character);
}

static void put_test_passage(uint8_t offset_x, uint8_t offset_y)
{
    put_test_tile(offset_x, offset_y, CHAR_PASSAGE);
    add_map_flags(BASE_X + offset_x, BASE_Y + offset_y, TILE_PASSAGE);
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
