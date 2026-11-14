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
