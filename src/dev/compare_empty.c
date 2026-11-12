// az üres keretprogram: a szintgeneráló fájlok helyett csonkok, hogy a keretprogram saját mérete levonható legyen (compare.bat)

#include <stdint.h>

#include "../level.h"
#include "../rng.h"
#include "../state.h"

void rng_init(void)
{
}

// az ellenőrzőösszeg által olvasott minden részbe ír: ha a játékállapot egy részét semmi nem írná, a fordító
// (az llvm-mos LTO-val, az Oscar64 az egész programot látva) az olvasását állandóra egyszerűsíthetné, és az üres keretprogram
// kisebb lenne a valódinál
void generate_level(void)
{
    uint8_t value = (uint8_t)game.seed;

    game.map_characters[0][0] = value;
    game.map_flags[0][0] = value;
    game.rooms[0].x = value;
    game.player.x = value;
    game.player.y = value;
}
