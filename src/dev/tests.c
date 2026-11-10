#include "dev.h"
#include "tests.h"

#include "../platform/screen.h"
#include "../state.h"

#define TEST_RESULT_COLUMN  21                  // az eredmény ebben az oszlopban kezdődik
#define TEST_PROGRESS_STEP  50                  // a hosszú teszt ennyi próbánként kiírja, hol tart

struct DevTest {
    const char *name;
    int (*fn)(uint16_t i);
    uint16_t count; // hány próba (1 = egyszeri teszt)
};

// gyors öntesztek: minden indításkor lefutnak
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
    { "MAP-FLAGS", test_map_flags, 1 },
};

// szint-önteszt: billentyűre
static const struct DevTest dev_level_tests[] = {
    { "LEVEL-GENERATION", test_level_generation, LEVEL_TEST_LEVEL_COUNT },
};

// a tesztek egymás után, soronként a név és az eredmény; igaz, ha mind sikerült
static bool run_test_table(const struct DevTest *tests, uint8_t test_count)
{
    bool all_ok = true;
    uint32_t saved_seed = game.seed;
    uint8_t saved_dungeon_level = game.dungeon_level;

    for (uint8_t t = 0; t < test_count; t++) {
        const struct DevTest *dt = &tests[t];
        uint16_t fails = 0;
        uint16_t first_fail = 0;

        set_screen_cursor_x(1);
        put_text(dt->name);

        for (uint16_t i = 0; i < dt->count; i++) {
            if (i % TEST_PROGRESS_STEP == 0 && dt->count > TEST_PROGRESS_STEP) {
                set_screen_cursor_x(TEST_RESULT_COLUMN);
                put_int(i, 0);
            }
            if (!dt->fn(i)) {
                if (fails == 0) {
                    first_fail = i;
                }
                fails++;
            }
        }

        set_screen_cursor_x(TEST_RESULT_COLUMN);
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
        clear_to_row_end();

        move_screen_cursor_to_next_row();
    }

    game.seed = saved_seed;
    game.dungeon_level = saved_dungeon_level;
    return all_ok;
}

bool dev_run_tests(void)
{
    return run_test_table(dev_tests, sizeof(dev_tests) / sizeof(dev_tests[0]));
}

// az arányok a szintek számlálóiból jönnek, ezért a generálás után
bool dev_run_level_tests(void)
{
    bool generation_ok = run_test_table(dev_level_tests, sizeof(dev_level_tests) / sizeof(dev_level_tests[0]));
    bool ratios_ok = show_level_room_ratios();
    return generation_ok && ratios_ok;
}
