#include "view.h"

#include <string.h>

#include "platform/platform.h"
#include "platform/screen.h"
#include "map_signs.h"
#include "state.h"

#define VIEW_WINDOW_WIDTH          SCREEN_WIDTH
#define VIEW_WINDOW_X_MAX          (MAP_COLUMNS - VIEW_WINDOW_WIDTH)
#define VIEW_WINDOW_EDGE_DISTANCE  8

static bool map_top_row_hidden;

void clear_seen_map(void)
{
    memset(game.seen_map, SOLID_ROCK, sizeof(game.seen_map));
}

void draw_tile(uint8_t map_x, uint8_t y, uint8_t ch)
{
    game.seen_map[y - 1][map_x] = ch;

    uint8_t column = map_x - game.view_window_x;
    if (column < VIEW_WINDOW_WIDTH && !(y == 1 && map_top_row_hidden)) {
        put_screen_character(column, y, ch);
    }
}

static void redraw_map_row(uint8_t y)
{
    screen_copy_row(y, game.seen_map[y - 1] + game.view_window_x);
}

void redraw_map(void)
{
    wait_for_next_frame();

    for (uint8_t y = map_top_row_hidden ? 2 : 1; y <= MAP_ROWS; y++) {
        redraw_map_row(y);
    }
}

void set_map_top_row_visibility(bool visible)
{
    map_top_row_hidden = !visible;

    if (visible) {
        redraw_map_row(1);
    }
}

static uint8_t calculate_centered_view_window_x(uint8_t map_x)
{
    if (map_x < VIEW_WINDOW_WIDTH / 2) {
        return 0;
    }

    uint8_t view_window_x = map_x - VIEW_WINDOW_WIDTH / 2;
    return view_window_x > VIEW_WINDOW_X_MAX ? VIEW_WINDOW_X_MAX : view_window_x;
}

uint8_t calculate_view_window_x_after_step(uint8_t map_x, uint8_t view_window_x)
{
    int8_t column = map_x - view_window_x;

    if (column < VIEW_WINDOW_EDGE_DISTANCE) {
        return map_x < VIEW_WINDOW_EDGE_DISTANCE ? 0 : map_x - VIEW_WINDOW_EDGE_DISTANCE;
    }

    if (column > VIEW_WINDOW_WIDTH - 1 - VIEW_WINDOW_EDGE_DISTANCE) {
        uint8_t new_view_window_x = map_x - (VIEW_WINDOW_WIDTH - 1 - VIEW_WINDOW_EDGE_DISTANCE);
        return new_view_window_x > VIEW_WINDOW_X_MAX ? VIEW_WINDOW_X_MAX : new_view_window_x;
    }

    return view_window_x;
}

void center_view_window(uint8_t map_x)
{
    game.view_window_x = calculate_centered_view_window_x(map_x);
    redraw_map();
}

void follow_view_window(uint8_t map_x)
{
    uint8_t view_window_x = calculate_view_window_x_after_step(map_x, game.view_window_x);

    if (view_window_x != game.view_window_x) {
        game.view_window_x = view_window_x;
        redraw_map();
    }
}
