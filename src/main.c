#include <stdio.h>
#include <c64/memmap.h>

#include "c64/platform.h"
#include "c64/memory.h"
#include "state.h"

int main(void)
{
    mmap_set(MMAP_NO_BASIC); // BASIC ROM off

    printf("ROGUE FOR C64\n");
    printf("(BASED ON ROGUE 5.4.4)\n");

    game.seed = 123;
    printf("SEED: %i", game.seed);

    mmap_set(MMAP_ROM); // BASIC ROM on
    return 0;
}
