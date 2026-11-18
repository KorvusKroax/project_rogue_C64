#include "dev.h"

#include <stddef.h>

#include "../platform/charset.h"
#include "../platform/key.h"
#include "../platform/screen.h"
#include "../explore.h"
#include "../input.h"
#include "../level.h"
#include "../map.h"
#include "../message.h"
#include "../monsters.h"
#include "../num.h"
#include "../state.h"
#include "../view.h"
#include "checks.h"

#define DEPTH_DIGITS_MAX  3         // 1..255

#define FULL_MAP_BORDER_COLOR  2   // piros: a teljes térkép be van kapcsolva

static bool full_map_shown = false;
static uint8_t full_map_row[VIEW_WINDOW_WIDTH];
static uint8_t normal_border_color;

// a titkos folyosó-mező a játékban üresnek, a titkos ajtó falnak látszik
static uint8_t choose_full_map_character(uint8_t character, uint8_t flags)
{
    if (flags & TILE_REAL) return character;
    return (flags & TILE_PASSAGE ? CHAR_PASSAGE : CHAR_DOOR) + INVERSE_OFFSET;
}

bool is_full_map_shown(void)
{
    return full_map_shown;
}

// csak a nézőablak oszlopai, közvetlenül a térkép soraiból: görgetéskor mind a 22 sor újraszámolódik
const uint8_t *build_full_map_row(uint8_t y)
{
    const uint8_t *characters = game.map_characters[y - MAP_TOP_ROW] + game.view_window_x;
    const uint8_t *flags = game.map_flags[y - MAP_TOP_ROW] + game.view_window_x;

    for (uint8_t column = 0; column < VIEW_WINDOW_WIDTH; column++) {
        full_map_row[column] = choose_full_map_character(characters[column], flags[column]);
    }

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        const struct Monster *monster = get_monster(index);
        uint8_t monster_column = monster->x - game.view_window_x;
        if (monster->y == y && monster_column < VIEW_WINDOW_WIDTH) {
            full_map_row[monster_column] = monster->type;
            if (monster->flags & MONSTER_RUNNING) {
                full_map_row[monster_column] += INVERSE_OFFSET;
            }
        }
    }

    uint8_t player_column = game.player.x - game.view_window_x;
    if (y == game.player.y && player_column < VIEW_WINDOW_WIDTH) {
        full_map_row[player_column] = CHAR_PLAYER;
    }
    return full_map_row;
}

static void toggle_full_map(void)
{
    full_map_shown = !full_map_shown;

    if (full_map_shown) {
        normal_border_color = get_border_color();
        set_border_color(FULL_MAP_BORDER_COLOR);
    } else {
        set_border_color(normal_border_color);
    }
    redraw_map();
}

// a beállítás bitjének átváltása, és az új állapot az üzenetsorba
static void toggle_option(uint8_t option, const char *name)
{
    options.flags ^= option;
    message_format_without_recall("\1: \2", name, (options.flags & option) ? "on" : "off", 0);
}

// ESC-re vagy hibás számra nem történik semmi
static void jump_to_depth(void)
{
    char answer[DEPTH_DIGITS_MAX + 1];
    uint32_t depth;

    bool answered = ask_text("depth (1-255): ", answer, DEPTH_DIGITS_MAX);
    message_clear();

    if (answered && convert_text_to_number(answer, DUNGEON_LEVEL_MAX, &depth) && depth >= 1) {
        game.dungeon_level = depth;
        new_level();
    }
}

// a játékos körüli első szabad mező (soronként, balról jobbra): járható, látszik a játékos mezőjéről, és nem áll rajta szörny
static bool find_free_neighbor(uint8_t *x, uint8_t *y)
{
    uint8_t player_x = game.player.x;
    uint8_t player_y = game.player.y;

    for (int8_t offset_y = -1; offset_y <= 1; offset_y++) {
        for (int8_t offset_x = -1; offset_x <= 1; offset_x++) {
            // a 0. oszlop (sor) előtti mező 255 lesz: azt az is_on_map kívülinek veszi
            uint8_t tile_x = player_x + offset_x;
            uint8_t tile_y = player_y + offset_y;
            if (!is_on_map(tile_x, tile_y)) continue;
            if (tile_x == player_x && tile_y == player_y) continue;
            if (!is_walkable_character(get_map_character(tile_x, tile_y))) continue;
            if (!is_neighbor_visible(player_x, player_y, tile_x, tile_y)) continue;
            if (get_map_flags(tile_x, tile_y) & TILE_MONSTER) continue;

            *x = tile_x;
            *y = tile_y;
            return true;
        }
    }
    return false;
}

// a szörny betűjének bekérése, és létrehozása a játékos mellett; ESC-re vagy nem betűre nem történik semmi
// utána a szörnytábla foglaltsága, előtte a hiba, ha nem jött létre; a kirajzolást a következő look_around végzi
static void create_monster_by_letter(void)
{
    char answer[2];
    bool answered = ask_text("monster (A-Z): ", answer, 1);
    message_clear();
    if (!answered) return;

    uint8_t type = answer[0];
    if (type >= 'a' && type <= 'z') {
        type -= 'a' - 'A';
    }
    if (type < 'A' || type > 'Z') return;

    const char *failure = "";
    uint8_t x;
    uint8_t y;

    if (game.monster_count == MAX_MONSTERS) {
        failure = "the monster table is full, ";
    } else if (!find_free_neighbor(&x, &y)) {
        failure = "no free tile, ";
    } else {
        create_monster(type, x, y);
        if (!are_monster_tiles_valid()) {
            failure = "monster tiles invalid, ";
        }
    }

    char max_monsters_text[FORMAT_BUFFER_SIZE];
    format_int(max_monsters_text, MAX_MONSTERS, 0);
    message_format_without_recall("\1monsters: \3/\2", failure, max_monsters_text, game.monster_count);
}

// a szint összes szörnye felébred és üldöz; utána a számuk az üzenetsorba
static void start_all_monsters_running(void)
{
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        start_monster_running(get_monster(index));
    }
    message_format_without_recall("running monsters: \3", NULL, NULL, game.monster_count);
}

// ha van nem lassított szörny, mindegyik lassított lesz, különben egyik sem; az új állapot az üzenetsorba
static void toggle_all_monsters_slow(void)
{
    bool has_unslowed_monster = false;
    for (uint8_t index = 1; index <= game.monster_count; index++) {
        if (!(get_monster(index)->flags & MONSTER_SLOW)) {
            has_unslowed_monster = true;
        }
    }

    for (uint8_t index = 1; index <= game.monster_count; index++) {
        struct Monster *monster = get_monster(index);
        if (has_unslowed_monster) {
            monster->flags |= MONSTER_SLOW;
        } else {
            monster->flags &= ~MONSTER_SLOW;
        }
    }
    message_format_without_recall("slow monsters: \1", has_unslowed_monster ? "on" : "off", NULL, 0);
}

bool dev_execute_command(uint8_t code)
{
    switch (code) {
        case KEY_F1:
            new_level();
            break;
        case KEY_F2:
            create_monster_by_letter();
            break;
        case KEY_F3:
            jump_to_depth();
            break;
        case KEY_F4:
            start_all_monsters_running();
            break;
        case KEY_F5:
            toggle_full_map();
            break;
        case KEY_F6:
            toggle_all_monsters_slow();
            break;
        case KEY_F7:
            toggle_option(OPTION_JUMP_TO_RUN_END, "jump to run end");
            break;
        case KEY_F8:
            toggle_option(OPTION_FOLLOW_PASSAGES, "follow passages");
            break;
    }

    return false;
}
