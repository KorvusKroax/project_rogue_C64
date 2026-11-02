#ifndef VIEW_H
#define VIEW_H

#include <stdint.h>
#include <stdbool.h>

void clear_seen_map(void);          // a látott térkép szóközzel feltöltése, nem rajzol

void draw_tile(uint8_t map_x, uint8_t y, uint8_t ch);   // a mező jele a látott térképre, és ha látszik, a videomemóriába; y: 1..MAP_ROWS

void redraw_map(void);              // a térképterület újrarajzolása a látott térképből

void set_map_top_row_visibility(bool visible);     // false: az 1. sort az üzenetsáv foglalja, a térkép addig nem rajzol oda; true: visszarajzolja

void center_view_window(uint8_t map_x);     // a nézőablak a map_x oszlopra áll, újrarajzolással
void follow_view_window(uint8_t map_x);     // görget, ha a map_x oszlop a holt zónán kívül van

uint8_t calculate_view_window_x_after_step(uint8_t map_x, uint8_t view_window_x);   // a nézőablak új helye, ha a követett oszlop map_x lett; nem rajzol

#endif
