#include <stdbool.h>

#include "platform/platform.h"
#include "platform/screen.h"
#include "game.h"
#include "rng.h"
#include "state.h"

#ifdef DEVMODE
    #include "dev/dev.h"
#endif

// nem időkritikus kód: méretre fordítva, itt a hely fontosabb a sebességnél
#pragma optimize(push)
#pragma optimize(size)

int main(void)
{
    platform_init();
    rng_init();

    game.seed = calculate_seed();

    #ifdef DEVMODE
        #ifdef DEV_SEED
            game.seed = DEV_SEED;
        #endif

        dev_run_start_screen();
    #endif

    screen_clear();
    set_screen_colors(0, 0, 15);

    // ideiglenes: a címképernyő és a játék vége (M10-01, M10-05) elkészültéig a játék közvetlenül indul, és halál után újra
    while (true) {
        run_game();
    }
}

#pragma optimize(pop)
