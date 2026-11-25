#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

static const struct DevTest equipment_tests[] = {
    { "EQUIP-WIELD", test_wield_item, 1 },
    { "EQUIP-WEAR", test_wear_armor, 1 },
    { "EQUIP-TAKE OFF", test_take_off_armor, 1 },
    { "EQUIP-DROP IN USE", test_drop_item_in_use, 1 },
    { "EQUIP-STARTING", test_starting_equipment, STARTING_EQUIPMENT_TEST_SEED_COUNT },
    { "EQUIP-IN USE", test_report_items_in_use, 1 },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a parancsok és az üres körben támadó szörny üzenetei használják; a teli sáv nem vár szóközre
    set_message_more_wait(false);
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Equipment tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(equipment_tests, TEST_COUNT(equipment_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
