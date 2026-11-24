#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../message.h"

static const struct DevTest combat_tests[] = {
    { "COMBAT-STRENGTH", test_strength_bonuses, 1 },
    { "COMBAT-NAMES", test_monster_names, 1 },
    { "COMBAT-PLAYER HITS", test_player_hit_rate, 1 },
    { "COMBAT-MONSTER HITS", test_monster_hit_rate, 1 },
    { "COMBAT-MOVE ATTACK", test_move_attack, 1 },
    { "COMBAT-MONST ATTACK", test_monster_attack, 1 },
    { "FIGHT-DIAGONAL", test_fight_diagonal, 1 },
    { "FIGHT-HIT LIMIT", test_fight_hit_limit, 1 },
    { "FIGHT-OTHER ATTACK", test_fight_other_attack, 1 },
    { "FIGHT-TARGET MOVES", test_fight_target_moves, 1 },
    { "FIGHT-TARGET DIES", test_fight_target_death, 1 },
    { "COMBAT-KILL", test_monster_kill, 1 },
    { "COMBAT-REMOVAL", test_monster_removal, 1 },
    { "XP-LEVELS", test_experience_levels, 1 },
    { "SPECIAL-ICE", test_ice_monster, 1 },
    { "SPECIAL-VAMPIRE", test_vampire, 1 },
    { "SPECIAL-WRAITH", test_wraith, 1 },
    { "SPECIAL-RATTLESNAKE", test_rattlesnake, 1 },
    { "PLAYER-STRENGTH", test_strength_change, 1 },
    { "SPECIAL-FLYTRAP", test_flytrap, 1 },
    { "SPECIAL-HELD", test_flytrap_hold, 1 },
    { "SPECIAL-XEROC", test_xeroc_reveal, 1 },
    { "PLAYER-DEATH", test_player_death, 1 },
    { "PLAYER-ARMOR", test_player_armor, 1 },
};

int main(void)
{
    init_test_program();

    // az üzenetsáv sorait a támadások üzenetei használják; a teli sáv nem vár szóközre
    set_message_more_wait(false);
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Combat tests:");
    move_screen_cursor_to_next_row();
    bool all_ok = run_test_table(combat_tests, TEST_COUNT(combat_tests));
    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
