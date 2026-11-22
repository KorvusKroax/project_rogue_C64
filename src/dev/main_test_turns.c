#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../items.h"
#include "../message.h"

// a tesztek a test_level.c-ben vannak, mert a szintteszt mélységeit, magjait és táblázatát használják
static const struct DevTest turn_tests[] = {
    { "LEVEL-MONST MOVES", test_level_monster_moves, LEVEL_MOVE_TEST_LEVEL_COUNT },
};

// a szörnyek lépései generált szinteken, és egy kör ideje; VICE-ben warp módban; ESC-re leáll, az eredmény az addigi szintekből jön
int main(void)
{
    init_test_program();

    set_screen_cursor(0, SCREEN_HEIGHT - 1);
    put_text("ESC: stop");
    // az üzenetsáv sorait a szörnyek támadásainak üzenetei használják; a teli sáv nem vár szóközre
    set_message_more_wait(false);
    // a szintek tárgyai a tárgytáblába kerülnek, és új szintnél onnan szabadulnak fel
    clear_item_table();
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Turn tests:");
    move_screen_cursor_to_next_row();

    // a kör ideje a szintek számlálóiból jön, ezért a teszt után
    bool all_ok = run_test_table(turn_tests, TEST_COUNT(turn_tests));
    show_level_turn_times();

    if (!all_ok) {
        put_failed_test_names("Test failed: ");
    }

    end_test_program(all_ok);
}
