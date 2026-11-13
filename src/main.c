#include <stdbool.h>

#include "platform/platform.h"
#include "platform/screen.h"
#include "game.h"
#include "rng.h"
#include "state.h"

#ifdef DEVMODE
    #include "dev/dev.h"
#endif

int main(void)
{
    platform_init();
    rng_init();

    game.seed = calculate_seed();

    #ifdef DEVMODE
        #ifdef DEV_SEED
            game.seed = DEV_SEED;
        #endif

        dev_run_screens();
    #endif

    screen_clear();
    set_screen_colors(0, 0, 15);

    // ideiglenes: a címképernyő elkészültéig a játék közvetlenül indul
    run_game();
}
