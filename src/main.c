#include <stdbool.h>

#include "platform/platform.h"
#include "platform/screen.h"
#include "dev/dev.h"
#include "rng.h"
#include "state.h"

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

    set_screen_cursor(0, 0);
    put_text("Rogue for C64");
    move_screen_cursor_to_next_row();
    put_text("(based on Rogue 5.4.4)");

    // ideiglenes: a címképernyő elkészültéig itt megáll
    while (true) {}
}
