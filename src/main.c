#include "platform/platform.h"
#include "platform/screen.h"
#include "platform/key.h"
#include "dev/dev.h"
#include "state.h"

int main(void)
{
    platform_init();

    game.seed = platform_seed();

    #ifdef DEVMODE
        #ifdef DEV_SEED
            game.seed = DEV_SEED;
        #endif

        screen_clear();

        screen_at(0, 0);
        put_text("Seed: ");
        put_uint(game.seed, 0);
        screen_newline();
        screen_newline();

        put_text("Tests:");
        screen_newline();
        if (!dev_run_tests()) {
            put_text("Selftest failed");
            screen_newline();
        }
        screen_newline();
        put_text("Press a key");
        while (!key_get()) {}

        dev_charset_screen();
        screen_at(2, 24);
        put_text("Press a key");
        while (!key_get()) {}

        dev_keyboard_screen();
    #endif

    screen_clear();
    screen_color(0, 0, 15);

    screen_at(0, 0);
    put_text("Rogue for C64");
    screen_newline();
    put_text("(based on Rogue 5.4.4)");

    while (!key_get()) {
        wait_frame();
    }

    platform_exit();
    return 0;
}
