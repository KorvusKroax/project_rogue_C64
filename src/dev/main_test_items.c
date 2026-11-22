#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

static const struct DevTest item_tests[] = {
    { "ITEM-CHANCES", test_item_chances, 1 },
    { "ITEM-NO FOOD", test_levels_without_food, 1 },
    { "ITEM-TABLE", test_item_table, 1 },
    { "ITEM-LISTS", test_item_lists, 1 },
    { "ITEM-INITIAL", test_item_initial_values, 1 },
    { "ITEM-GROUPS", test_item_groups, 1 },
    { "ITEM-NAME LISTS", test_item_name_lists, 1 },
    { "ITEM-PSEUDONYMS", test_item_pseudonyms, 1 },
    { "ITEM-SCROLL TITLES", test_scroll_titles, SCROLL_TITLE_TEST_SEED_COUNT },
    { "ITEM-NAMES", test_item_names, 1 },
    { "ITEM-LONGEST NAME", test_longest_item_name, 1 },
    { "PACK-SLOTS", test_pack_slots, 1 },
    { "PACK-LETTERS", test_pack_letters, 1 },
    { "PACK-ORDER", test_pack_order, 1 },
    { "PICKUP-STEP", test_pickup_by_step, 1 },
    { "PICKUP-GOLD", test_pickup_gold, 1 },
    { "PICKUP-FULL PACK", test_pickup_full_pack, 1 },
    { "DROP-STACK PIECE", test_drop_stack_piece, 1 },
    { "DROP-FULL TABLE", test_drop_full_table, 1 },
    { "AMULET-CARRIED", test_amulet_carried, 1 },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a felvétel és a lerakás üzenetei használják; a teli sáv nem vár szóközre
    set_message_more_wait(false);
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Item tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(item_tests, TEST_COUNT(item_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
