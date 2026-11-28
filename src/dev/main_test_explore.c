#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

static const struct DevTest explore_tests[] = {
    { "MOVE-DIAGONAL", test_diagonal_step, 1 },
    { "LOOK-NEIGHBORS", test_neighbor_visibility, 1 },
    { "RUN-CAREFUL", test_careful_run, 1 },
    { "RUN-PASSAGE TURN", test_passage_turn, 1 },
    { "RUN-BLIND", test_blind_run, 1 },
    { "SEARCH-HIDDEN", test_search, 1 },
};

int main(void)
{
    init_test_program();

    put_text("Explore tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(explore_tests, TEST_COUNT(explore_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
