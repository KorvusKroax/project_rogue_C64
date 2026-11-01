#include "view.h"

#include "platform/platform.h"
#include "platform/screen.h"
#include "state.h"

#define VIEW_W         SCREEN_W
#define VIEW_X_MAX     (NUMCOLS - VIEW_W)
#define SCROLL_MARGIN  8

static bool top_row_hidden;

void draw_tile(uint8_t x, uint8_t y, uint8_t ch)
{
    game.screen[y - 1][x] = ch;

    uint8_t column = x - game.view_x;
    if (column < VIEW_W && !(y == 1 && top_row_hidden)) {
        screen_put(column, y, ch);
    }
}

static void redraw_row(uint8_t y)
{
    screen_copy_row(y, game.screen[y - 1] + game.view_x);
}

void redraw_map(void)
{
    wait_frame();

    for (uint8_t y = top_row_hidden ? 2 : 1; y <= MAP_ROWS; y++) {
        redraw_row(y);
    }
}

void view_hide_top_row(bool hidden)
{
    top_row_hidden = hidden;

    if (!hidden) {
        redraw_row(1);
    }
}

static uint8_t centered_view_x(uint8_t x)
{
    if (x < VIEW_W / 2) {
        return 0;
    }

    uint8_t view_x = x - VIEW_W / 2;
    return view_x > VIEW_X_MAX ? VIEW_X_MAX : view_x;
}

uint8_t view_x_after_step(uint8_t x, uint8_t view_x)
{
    int8_t column = x - view_x;

    if ((column < SCROLL_MARGIN && view_x > 0) ||
        (column > VIEW_W - 1 - SCROLL_MARGIN && view_x < VIEW_X_MAX)) {
        return centered_view_x(x);
    }

    return view_x;
}

void center_view(uint8_t x)
{
    game.view_x = centered_view_x(x);
    redraw_map();
}

void follow_view(uint8_t x)
{
    uint8_t view_x = view_x_after_step(x, game.view_x);

    if (view_x != game.view_x) {
        game.view_x = view_x;
        redraw_map();
    }
}
