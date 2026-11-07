#include "tests.h"

#include <stdbool.h>

#include "../map.h"
#include "../state.h"

#define LAST_COLUMN  (MAP_COLUMNS - 1)
#define LAST_ROW     (MAP_TOP_ROW + MAP_ROWS - 1)

static bool is_cleared_tile(uint8_t x, uint8_t y)
{
    return get_map_character(x, y) == CHAR_SOLID_ROCK && get_map_flags(x, y) == TILE_REAL;
}

int test_map_flags(uint16_t i)
{
    // törlés után a térkép négy sarka és közepe üres, nem rejtett mező
    clear_map();
    if (!is_cleared_tile(0, MAP_TOP_ROW) || !is_cleared_tile(LAST_COLUMN, MAP_TOP_ROW)) return false;
    if (!is_cleared_tile(0, LAST_ROW) || !is_cleared_tile(LAST_COLUMN, LAST_ROW)) return false;
    if (!is_cleared_tile(MAP_COLUMNS / 2, MAP_TOP_ROW + MAP_ROWS / 2)) return false;

    // a jel és a flagek egymástól függetlenek
    uint8_t x = MAP_COLUMNS / 2;
    uint8_t y = MAP_TOP_ROW + MAP_ROWS / 2;
    set_map_character(x, y, CHAR_PASSAGE);
    if (get_map_character(x, y) != CHAR_PASSAGE || get_map_flags(x, y) != TILE_REAL) return false;

    // bekapcsolás: a többi flag marad
    add_map_flags(x, y, TILE_PASSAGE | TILE_SEEN);
    if (get_map_flags(x, y) != (TILE_REAL | TILE_PASSAGE | TILE_SEEN)) return false;

    // kikapcsolás: a többi flag marad
    remove_map_flags(x, y, TILE_REAL);
    if (get_map_flags(x, y) != (TILE_PASSAGE | TILE_SEEN)) return false;

    // a sorszám mezője nem írja felül a jelzőket, és vissza is olvasható
    add_map_flags(x, y, TILE_PASSAGE_NUMBER);
    if ((get_map_flags(x, y) & ~TILE_PASSAGE_NUMBER) != (TILE_PASSAGE | TILE_SEEN)) return false;
    remove_map_flags(x, y, TILE_PASSAGE_NUMBER);
    if (get_map_flags(x, y) != (TILE_PASSAGE | TILE_SEEN)) return false;

    // a szomszédos mezők és a jelük változatlan
    if (!is_cleared_tile(x - 1, y) || !is_cleared_tile(x + 1, y)) return false;
    if (!is_cleared_tile(x, y - 1) || !is_cleared_tile(x, y + 1)) return false;
    if (get_map_character(x, y) != CHAR_PASSAGE) return false;

    // a szörny jelzője egyedül is be- és kikapcsolható
    add_map_flags(x, y, TILE_MONSTER);
    if (!(get_map_flags(x, y) & TILE_MONSTER)) return false;
    remove_map_flags(x, y, TILE_MONSTER);
    if (get_map_flags(x, y) & TILE_MONSTER) return false;

    clear_map();
    return true;
}
