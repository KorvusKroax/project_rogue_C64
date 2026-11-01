#include "view.h"

#include "platform/screen.h"
#include "state.h"

#define VIEW_W  SCREEN_W

void draw_tile(uint8_t x, uint8_t y, uint8_t ch)
{
    game.screen[y - 1][x] = ch;

    uint8_t column = x - game.view_x;
    if (column < VIEW_W) {
        screen_put(column, y, ch);
    }
}
