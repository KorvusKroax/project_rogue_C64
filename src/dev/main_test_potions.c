#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

#define POTION_VALUE_TEST_SEED_COUNT  50       // ennyi seeddel: a próbánként változó szint és hiány a gyógyítás mindkét ágát elővezeti

static const struct DevTest potion_tests[] = {
    { "TIMER-EXPIRY", test_timer_expiry, 1 },
    { "POTION-QUAFF", test_quaff_item, 1 },
    { "POTION-MESSAGES", test_potion_messages, 1 },
    { "MAGIC-DETECTION", test_magic_detection, 1 },
    { "POTION-DURATIONS", test_potion_durations, 1 },
    { "POTION-VALUES", test_potion_values, POTION_VALUE_TEST_SEED_COUNT },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a bájitalok és a lejáratok üzenetei használják; a teli sáv nem vár szóközre
    set_message_more_wait(false);
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Potion tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(potion_tests, TEST_COUNT(potion_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
