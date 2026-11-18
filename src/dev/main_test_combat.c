#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"

static const struct DevTest combat_tests[] = {
    { "COMBAT-STRENGTH", test_strength_bonuses, 1 },
    { "COMBAT-PLAYER HITS", test_player_hit_rate, 1 },
    { "COMBAT-MONSTER HITS", test_monster_hit_rate, 1 },
    { "COMBAT-MOVE ATTACK", test_move_attack, 1 },
    { "COMBAT-MONST ATTACK", test_monster_attack, 1 },
    { "FIGHT-DIAGONAL", test_fight_diagonal, 1 },
    { "FIGHT-HIT LIMIT", test_fight_hit_limit, 1 },
    { "FIGHT-OTHER ATTACK", test_fight_other_attack, 1 },
    { "FIGHT-TARGET MOVES", test_fight_target_moves, 1 },
};

int main(void)
{
    init_test_program();

    put_text("Combat tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(combat_tests, TEST_COUNT(combat_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
