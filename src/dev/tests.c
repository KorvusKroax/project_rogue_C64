#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

// gyors öntesztek: minden indításkor lefutnak
static const struct DevTest dev_tests[] = {
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
    { "MAP-FLAGS", test_map_flags, 1 },
    { "MOVE-DIAGONAL", test_diagonal_step, 1 },
    { "LOOK-NEIGHBORS", test_neighbor_visibility, 1 },
    { "RUN-CAREFUL", test_careful_run, 1 },
    { "RUN-PASSAGE TURN", test_passage_turn, 1 },
    { "SEARCH-HIDDEN", test_search, 1 },
    { "MONST-TYPES", test_monster_types, 1 },
    { "MONST-TABLE", test_monster_table, 1 },
    { "MONST-ORDER", test_monster_order, 1 },
    { "MONST-GENERATE", test_monster_generation, 1 },
    { "MONST-TILES", test_monster_tiles, 1 },
    { "MONST-SEE", test_monster_visibility, 1 },
};

// szint-önteszt: billentyűre
static const struct DevTest dev_level_tests[] = {
    { "LEVEL-GENERATION", test_level_generation, LEVEL_TEST_LEVEL_COUNT },
};

bool dev_run_tests(void)
{
    return run_test_table(dev_tests, TEST_COUNT(dev_tests));
}

// az arányok és az idők a szintek számlálóiból jönnek, ezért a generálás után
bool dev_run_level_tests(void)
{
    bool generation_ok = run_test_table(dev_level_tests, TEST_COUNT(dev_level_tests));
    bool ratios_ok = show_level_room_ratios();
    move_screen_cursor_to_next_row();
    bool times_ok = show_level_generation_times();
    return generation_ok && ratios_ok && times_ok;
}
