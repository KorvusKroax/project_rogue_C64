#include "tests.h"
#include "test_runner.h"

#include "../platform/screen.h"
#include "../items.h"
#include "../message.h"

static const struct DevTest level_tests[] = {
    { "LEVEL-GENERATION", test_level_generation, LEVEL_TEST_LEVEL_COUNT },
    { "LEVEL-AMULET LOOT", test_level_amulet_loot, LEVEL_AMULET_TEST_LEVEL_COUNT },
};

// hosszú (normál sebességen percekig), VICE-ben warp módban; ESC-re leáll, az eredmény az addigi szintekből jön
int main(void)
{
    init_test_program();

    set_screen_cursor(0, SCREEN_HEIGHT - 1);
    put_text("ESC: stop");
    // a szintek tárgyai a tárgytáblába kerülnek, és új szintnél onnan szabadulnak fel
    clear_item_table();
    set_screen_cursor(0, MESSAGE_ROW + MESSAGE_ROW_COUNT);
    put_text("Level tests:");
    move_screen_cursor_to_next_row();

    // az arányok és az idők a szintek számlálóiból jönnek, ezért a generálás után
    bool generation_ok = run_test_table(level_tests, TEST_COUNT(level_tests));
    bool ratios_ok = show_level_room_ratios();
    bool times_ok = show_level_generation_times();

    bool all_ok = generation_ok && ratios_ok && times_ok;
    if (!all_ok) {
        put_text("Level test failed");
    }

    end_test_program(all_ok);
}
