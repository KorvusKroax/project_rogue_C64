#include "dev.h"
#include "tests.h"

#include "../platform/screen.h"
#include "../state.h"

struct DevTest {
    const char *name;
    int (*fn)(uint16_t i);
    uint16_t count; // hány próba (1 = egyszeri teszt)
};

static const struct DevTest dev_tests[] = {
    { "RNG-RN", test_RN, 1 },
    { "RNG-RND", test_rnd, 1 },
    { "RNG-ROLL", test_ROLL, 1 },
    { "RNG-SPREAD", test_spread, 1 },
    { "NUM-SAT ARITHMETIC", test_saturating_arithmetic, 1 },
    { "NUM-FORMAT", test_format, 1 },
    { "KEY-DECODE", test_key_decode, 1 },
    { "VIEW-FOLLOW", test_view_follow, 1 },
    { "MESS-PREPARE TEXT", test_prepare_message_text, 1 },
    { "MESS-LINE LENGTH", test_message_chars_fitting_row, 1 },
    { "MESS-PAGE LENGTH", test_message_chars_fitting_bar, 1 },
};

bool dev_run_tests(void)
{
    bool all_ok = true;
    uint32_t saved_seed = game.seed;

    for (uint8_t t = 0; t < sizeof(dev_tests) / sizeof(dev_tests[0]); t++) {
        const struct DevTest *dt = &dev_tests[t];
        uint16_t fails = 0, first_fail = 0;

        set_screen_cursor_x(1);
        put_text(dt->name);
        set_screen_cursor_x(21);

        for (uint16_t i = 0; i < dt->count; i++) {
            if (!dt->fn(i)) {
                if (fails == 0) {
                    first_fail = i;
                }
                fails++;
            }
        }

        if (fails == 0) {
            if (dt->count == 1) {
                put_text("OK");
            } else {
                put_int(dt->count, 0);
                put_text("/");
                put_int(dt->count, 0);
            }
        } else {
            put_int(dt->count - fails, 0);
            put_text("/");
            put_int(dt->count, 0);
            put_text(" FAIL AT ");
            put_int(first_fail, 0);
            all_ok = false;
        }

        move_screen_cursor_to_next_row();
    }

    game.seed = saved_seed;
    return all_ok;
}
