#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

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
};

int main(void)
{
    init_test_program();

    put_text("Item tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(item_tests, TEST_COUNT(item_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
