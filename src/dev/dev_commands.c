#include "dev.h"

#include "../platform/charset.h"
#include "../platform/key.h"
#include "../platform/screen.h"
#include "../input.h"
#include "../level.h"
#include "../map.h"
#include "../message.h"
#include "../state.h"
#include "../view.h"

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

uint8_t calculate_full_map_character(uint8_t x, uint8_t y)
{
    return choose_full_map_character(get_map_character(x, y), get_map_flags(x, y));
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

// ESC-re vagy hibás számra nem történik semmi
static void jump_to_depth(void)
{
    char answer[DEPTH_DIGITS_MAX + 1];
    uint32_t depth;

    bool answered = ask_text("depth (1-255): ", answer, DEPTH_DIGITS_MAX);
    message_clear();

    if (answered && convert_text_to_number(answer, 255, &depth) && depth >= 1) {
        game.dungeon_level = depth;
        new_level();
    }
}

bool dev_execute_command(uint8_t code)
{
    switch (code) {
        case KEY_F1:
            new_level();
            break;
        case KEY_F3:
            jump_to_depth();
            break;
        case KEY_F5:
            toggle_full_map();
            break;
    }

    return false;
}
