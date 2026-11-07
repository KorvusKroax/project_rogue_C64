#include "view.h"

#include <string.h>

#include "platform/platform.h"
#include "platform/screen.h"
#include "map_signs.h"
#include "message.h"
#include "state.h"
#include "status.h"

// görgethető nézőablakban a két szél holt zónája között maradnia kell helynek, különben a nézőablak minden lépésnél ide-oda ugrana
#if VIEW_WINDOW_X_MAX > 0 && 2 * VIEW_WINDOW_EDGE_DISTANCE >= VIEW_WINDOW_WIDTH
    #error "a VIEW_WINDOW_EDGE_DISTANCE túl nagy a nézőablakhoz"
#endif

// a logikai y sor a képernyő y. sorában van: a térkép az üzenetsor első sora és a státuszsor közé kell férjen
#if MAP_TOP_ROW <= MESSAGE_ROW || MAP_TOP_ROW + MAP_ROWS > STATUS_ROW
    #error "a térkép nem fér el az üzenetsor és a státuszsor között"
#endif

#define MESSAGE_SECOND_ROW  (MESSAGE_ROW + 1)   // a térkép e sorát az üzenetsáv ideiglenesen eltakarhatja

static bool map_top_row_hidden;

void clear_seen_map(void)
{
    memset(game.seen_map, SOLID_ROCK, sizeof(game.seen_map));
}

void draw_tile(uint8_t map_x, uint8_t y, uint8_t ch)
{
    game.seen_map[y - MAP_TOP_ROW][map_x] = ch;

    uint8_t column = map_x - game.view_window_x;
    if (column < VIEW_WINDOW_WIDTH && !(y == MESSAGE_SECOND_ROW && map_top_row_hidden)) {
        put_screen_character(column, y, ch);
    }
}

static void redraw_map_row(uint8_t y)
{
    screen_copy_row(y, game.seen_map[y - MAP_TOP_ROW] + game.view_window_x, VIEW_WINDOW_WIDTH);
}

void redraw_map(void)
{
    wait_for_next_frame();

    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        if (!(y == MESSAGE_SECOND_ROW && map_top_row_hidden)) {
            redraw_map_row(y);
        }
    }
}

void set_map_top_row_visibility(bool visible)
{
    map_top_row_hidden = !visible;

    if (visible) {
        redraw_map_row(MESSAGE_SECOND_ROW);
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
