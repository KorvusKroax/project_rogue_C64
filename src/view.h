#ifndef VIEW_H
#define VIEW_H

#include <stdint.h>
#include <stdbool.h>

#include "platform/screen.h"
#include "state.h"

// a nézőablak a képernyő teljes szélessége; a képernyőnél keskenyebb térképnél a térkép szélessége, és nem görget
#if MAP_COLUMNS < SCREEN_WIDTH
    #define VIEW_WINDOW_WIDTH  MAP_COLUMNS
#else
    #define VIEW_WINDOW_WIDTH  SCREEN_WIDTH
#endif

#define VIEW_WINDOW_X_MAX          (MAP_COLUMNS - VIEW_WINDOW_WIDTH)   // a nézőablak bal szélének legnagyobb oszlopa
#define VIEW_WINDOW_EDGE_DISTANCE  8                                   // görgetés, ha a követett oszlop ennél közelebb ér a széléhez

void clear_seen_map(void);                              // a látott térkép szóközzel feltöltése, nem rajzol

void draw_tile(uint8_t map_x, uint8_t y, uint8_t ch);   // a mező jele a látott térképre, és ha látszik, a videomemóriába; y: MAP_TOP_ROW..MAP_TOP_ROW+MAP_ROWS-1

void redraw_map(void);                                  // a térképterület újrarajzolása a látott térképből

void set_map_first_visible_row(uint8_t row);            // a térkép csak a row. képernyősortól rajzol, a fölötte lévőket az üzenetsáv foglalja; a felszabaduló sorokat visszarajzolja; MAP_TOP_ROW vagy kisebb: semmi nincs takarva

void center_view_window(uint8_t map_x);                 // a nézőablak a map_x oszlopra áll, újrarajzolással
void follow_view_window(uint8_t map_x);                 // görget, ha a map_x oszlop a holt zónán kívül van

uint8_t calculate_view_window_x_after_step(uint8_t map_x, uint8_t view_window_x);   // a nézőablak új helye, ha a követett oszlop map_x lett; nem rajzol

#endif
