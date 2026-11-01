#ifndef VIEW_H
#define VIEW_H

#include <stdint.h>
#include <stdbool.h>

void view_clear(void);              // a SCREEN réteg szóközzel feltöltése, nem rajzol

void draw_tile(uint8_t x, uint8_t y, uint8_t ch);   // a mező jele a SCREEN rétegbe, és ha látszik, a videomemóriába; y: 1..MAP_ROWS

void redraw_map(void);              // a térképterület újrarajzolása a SCREEN rétegből

void view_hide_top_row(bool hidden);    // az 1. sort az üzenetsáv foglalja: a térkép addig nem rajzol oda; felszabadításkor visszarajzolja

void center_view(uint8_t x);        // a nézőablak az x oszlopra áll, újrarajzolással
void follow_view(uint8_t x);        // görget, ha az x oszlop a holt zónán kívül van

uint8_t view_x_after_step(uint8_t x, uint8_t view_x);   // a nézőablak új helye, ha a követett oszlop x lett; nem rajzol

#endif
