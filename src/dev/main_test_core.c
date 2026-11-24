#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

static const struct DevTest core_tests[] = {
    { "RNG-RN", test_RN, 1 },
    { "RNG-RND", test_rnd, 1 },
    { "RNG-ROLL", test_ROLL, 1 },
    { "RNG-SPREAD", test_spread, 1 },
    { "NUM-SAT ARITHMETIC", test_saturating_arithmetic, 1 },
    { "NUM-FORMAT", test_format, 1 },
    { "NUM-FROM TEXT", test_convert_text_to_number, 1 },
    { "KEY-DECODE", test_key_decode, 1 },
    { "VIEW-FOLLOW", test_view_follow, 1 },
    { "MESS-PREPARE TEXT", test_prepare_message_text, 1 },
    { "MESS-LINE LENGTH", test_message_chars_fitting_row, 1 },
    { "MESS-PAGE LENGTH", test_message_chars_fitting_bar, 1 },
    { "LIST-ROWS", test_text_list_rows, 1 },
    { "LIST-WINDOW FIT", test_text_list_window_fit, 1 },
    { "WINDOW-RESTORE", test_window_restore, 1 },
    { "LIST-ROW NOT JOINED", test_list_row_not_joined, 1 },
    { "MESS-PROMPT INPUT", test_prompt_input_space, 1 },
    { "MAP-FLAGS", test_map_flags, 1 },
    { "PLATFORM-COPY TEXT", test_copy_text, 1 },
};

int main(void)
{
    init_test_program();

    put_text("Core tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(core_tests, TEST_COUNT(core_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
