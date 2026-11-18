#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

static const struct DevTest monster_tests[] = {
    { "MONST-TYPES", test_monster_types, 1 },
    { "MONST-TABLE", test_monster_table, 1 },
    { "MONST-ORDER", test_monster_order, 1 },
    { "MONST-GENERATE", test_monster_generation, 1 },
    { "MONST-TILES", test_monster_tiles, 1 },
    { "MONST-SEE", test_monster_visibility, 1 },
    { "MONST-WAKE", test_monster_waking, 1 },
};

int main(void)
{
    init_test_program();

    put_text("Monster tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(monster_tests, TEST_COUNT(monster_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
