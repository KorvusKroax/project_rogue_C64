#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

static const struct DevTest monster_tests[] = {
    { "MONST-TYPES", test_monster_types, 1 },
    { "MONST-TABLE", test_monster_table, 1 },
    { "MONST-ORDER", test_monster_order, 1 },
    { "MONST-GENERATE", test_monster_generation, 1 },
    { "MONST-TILES", test_monster_tiles, 1 },
    { "MONST-SEE", test_monster_visibility, 1 },
    { "TILE-SEE", test_tile_visibility, 1 },
    { "BLIND-SEE", test_blind_view, 1 },
    { "MONST-WAKE", test_monster_waking, 1 },
    { "MONST-MEDUSA", test_medusa_gaze, 1 },
    { "MONST-CHASE", test_monster_chase, 1 },
    { "MONST-DESTINATION", test_monster_destination, 1 },
    { "MONST-PICKUP", test_monster_pickup, 1 },
    { "MONST-DEST TAKEN", test_destination_taken, 1 },
    { "MONST-GREEDY", test_greedy_monster, 1 },
    { "MONST-VANISH", test_vanishing_monster_turn, 1 },
    { "MONST-DROP LIT", test_lit_room_drops, 1 },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a szörnyek támadásainak üzenetei használják (üldözés); a teli sáv nem vár szóközre
    set_message_more_wait(false);
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Monster tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(monster_tests, TEST_COUNT(monster_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
