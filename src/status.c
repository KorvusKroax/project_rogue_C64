#include "status.h"

#include "platform/screen.h"
#include "player.h"
#include "state.h"

// a hunger_state szerint
static const char *const hunger_words[] = { "", "Hungry", "Weak", "Faint" };

void draw_status(void)
{
    set_screen_cursor(0, STATUS_ROW);
    put_text("Level:");
    put_int(game.dungeon_level, 0);
    put_text(" Gold:");
    put_int(game.gold, 0);
    put_text(" Hp:");
    put_int(game.player.hit_points, 0);
    put_text("(");
    put_int(game.player.max_hit_points, 0);
    put_text(") ");
    put_text(hunger_words[game.hunger_state]);
    clear_to_row_end();

    set_screen_cursor(0, STATUS_ROW + 1);
    put_text("Str:");
    put_int(game.player.strength, 0);
    put_text("(");
    put_int(game.max_strength, 0);
    put_text(") Arm:");
    put_int(BASE_ARMOR - calculate_player_armor(), 0);     // a páncél nélküli védelemhez képest, így a nagyobb a jobb
    put_text(" Exp:");
    put_int(game.player.experience_level, 0);
    put_text("/");
    put_int(game.player.experience, 0);
    clear_to_row_end();
}
