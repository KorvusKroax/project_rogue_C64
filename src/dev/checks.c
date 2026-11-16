#include "checks.h"

#include <stdint.h>

#include "../map.h"
#include "../monsters.h"
#include "../state.h"

bool are_monster_tiles_valid(void)
{
    uint16_t flagged_tile_count = 0;

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            if (get_map_flags(x, y) & TILE_MONSTER) {
                flagged_tile_count++;
            }
        }
    }

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        if (!(get_map_flags(monster->x, monster->y) & TILE_MONSTER)) return false;
    }

    // ha két szörny egy mezőn állna, kevesebb jelölt mező lenne, mint szörny
    return flagged_tile_count == game.monster_count;
}
