#include "map.h"

#include <string.h>

#include "state.h"

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
    game.map_characters[y - MAP_TOP_ROW][x] = character;
}

uint8_t get_map_flags(uint8_t x, uint8_t y)
{
    return game.map_flags[y - MAP_TOP_ROW][x];
}

void add_map_flags(uint8_t x, uint8_t y, uint8_t flags)
{
    game.map_flags[y - MAP_TOP_ROW][x] |= flags;
}

void remove_map_flags(uint8_t x, uint8_t y, uint8_t flags)
{
    game.map_flags[y - MAP_TOP_ROW][x] &= ~flags;
}
