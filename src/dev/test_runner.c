#include "test_runner.h"

#include "../platform/key.h"
#include "../platform/screen.h"
#include "../state.h"

#define TEST_RESULT_COLUMN  21                  // az eredmény ebben az oszlopban kezdődik

// a hibás tesztek neve a lista utáni kiíráshoz, mert a lista eleje kigörgethet; ennél többet nem jegyez meg
#define FAILED_TEST_NAMES_MAX  8

static const char *failed_test_names[FAILED_TEST_NAMES_MAX];
static uint8_t failed_test_count;

bool run_test_table(const struct DevTest *tests, uint8_t test_count)
{
    bool all_ok = true;
    failed_test_count = 0;
    uint32_t saved_seed = game.seed;
    uint8_t saved_dungeon_level = game.dungeon_level;

    for (uint8_t t = 0; t < test_count; t++) {
        const struct DevTest *dt = &tests[t];
        uint16_t fails = 0;
        uint16_t first_fail = 0;

        set_screen_cursor_x(1);
        put_text(dt->name);

        uint16_t tried = 0;
        while (tried < dt->count) {
            // a többpróbás teszt minden próba előtt kiírja, hányadiknál tart és mennyi van összesen,
            // és ESC-re abbahagyja: az eredmény az addigi próbákból jön
            if (dt->count > 1) {
                set_screen_cursor_x(TEST_RESULT_COLUMN);
                put_int(tried, 0);
                put_text("/");
                put_int(dt->count, 0);
                if (read_key() == KEY_ESCAPE) {
                    break;
                }
            }
            if (!dt->fn(tried)) {
                if (fails == 0) {
                    first_fail = tried;
                }
                fails++;
            }
            tried++;
        }

        set_screen_cursor_x(TEST_RESULT_COLUMN);
        if (fails == 0) {
            if (dt->count == 1) {
                put_text("OK");
            } else {
                put_int(tried, 0);
                put_text("/");
                put_int(dt->count, 0);
                if (tried < dt->count) {
                    put_text(" STOP");
                }
            }
        } else {
            put_int(tried - fails, 0);
            put_text("/");
            put_int(tried, 0);
            put_text(" FAIL AT ");
            put_int(first_fail, 0);
            all_ok = false;
            if (failed_test_count < FAILED_TEST_NAMES_MAX) {
                failed_test_names[failed_test_count++] = dt->name;
            }
        }
        clear_to_row_end();

        move_screen_cursor_to_next_row();
    }

    game.seed = saved_seed;
    game.dungeon_level = saved_dungeon_level;
    return all_ok;
}

void put_failed_test_names(const char *prefix)
{
    for (uint8_t i = 0; i < failed_test_count; i++) {
        put_text(prefix);
        put_text(failed_test_names[i]);
        move_screen_cursor_to_next_row();
    }
}
