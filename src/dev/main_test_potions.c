#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

static const struct DevTest potion_tests[] = {
    { "TIMER-EXPIRY", test_timer_expiry, 1 },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a lejáratok üzenetei használják; a teli sáv nem vár szóközre
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
