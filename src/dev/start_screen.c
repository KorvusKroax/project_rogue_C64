#include "dev.h"

#include "../platform/key.h"
#include "../platform/screen.h"
#include "../input.h"
#include "../num.h"
#include "../state.h"

// ritkán futó fejlesztői kód: méretre fordítva, a sebessége nem számít
#pragma optimize(push)
#pragma optimize(size)

// a seed előjel nélküli, a put_int előjeles: az utolsó jegy külön, így a többi elfér 31 biten
static void put_seed(uint32_t seed)
{
    if (seed >= 10) {
        put_int(seed / 10, 0);
    }
    put_int(seed % 10, 0);
}

// a seed kézi megadása a sávban; nem szám vagy ESC esetén marad a régi
static void ask_seed(void)
{
    char answer[FORMAT_DIGITS_MAX + 1];

    if (ask_text("new seed: ",answer, FORMAT_DIGITS_MAX)) {
        convert_text_to_number(answer, UINT32_MAX, &game.seed);
    }
}

static void draw_start_screen(void)
{
    screen_clear();

    set_screen_cursor(0, 2);
    put_text("Seed: ");
    put_seed(game.seed);
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();

    put_text("S: enter new seed");
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();
    put_text("RETURN: start the game");
}

void dev_run_start_screen(void)
{
    uint8_t code = 0;

    enable_function_keys();     // a fejlesztői parancsokhoz

    while (code != KEY_RETURN) {
        draw_start_screen();
        code = wait_for_key();

        if (code == 's') {
            ask_seed();
        }
    }
}

#pragma optimize(pop)
