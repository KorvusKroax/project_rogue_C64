#include "map.h"

#include <string.h>

#include "state.h"

// ideiglenes: amíg a térképre író kód (szörnyek, tárgyak, varázslatok) el nem készül
// a térképen kívüli írás a játékállapot más részét rontaná el; az olvasás nincs ellenőrizve, az csak rossz értéket ad
#ifdef DEVMODE
    #include "dev/dev.h"

    #define CHECK_MAP_WRITE_POSITION(x, y) \
        DEV_CHECK((x) < MAP_COLUMNS && (y) >= MAP_TOP_ROW && (y) < MAP_TOP_ROW + MAP_ROWS, "map write outside the map")
#else
    #define CHECK_MAP_WRITE_POSITION(x, y) ((void)0)
#endif

void clear_map(void)
{
    memset(game.map_characters, CHAR_SOLID_ROCK, sizeof(game.map_characters));
    memset(game.map_flags, TILE_REAL, sizeof(game.map_flags));
}

uint8_t get_map_character(uint8_t x, uint8_t y)
{
    return game.map_characters[y - MAP_TOP_ROW][x];
}

void set_map_character(uint8_t x, uint8_t y, uint8_t character)
{
    CHECK_MAP_WRITE_POSITION(x, y);
    game.map_characters[y - MAP_TOP_ROW][x] = character;
}

uint8_t get_map_flags(uint8_t x, uint8_t y)
{
    return game.map_flags[y - MAP_TOP_ROW][x];
}

void add_map_flags(uint8_t x, uint8_t y, uint8_t flags)
{
    CHECK_MAP_WRITE_POSITION(x, y);
    game.map_flags[y - MAP_TOP_ROW][x] |= flags;
}

void remove_map_flags(uint8_t x, uint8_t y, uint8_t flags)
{
    CHECK_MAP_WRITE_POSITION(x, y);
    game.map_flags[y - MAP_TOP_ROW][x] &= ~flags;
}

bool is_walkable_character(uint8_t character)
{
    return character != CHAR_SOLID_ROCK && character != CHAR_WALL_HORIZONTAL && character != CHAR_WALL_VERTICAL;
}
