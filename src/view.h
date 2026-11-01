#ifndef VIEW_H
#define VIEW_H

#include <stdint.h>

// a mező jele a SCREEN rétegbe, és ha látszik, a videomemóriába; y: 1..MAP_ROWS
void draw_tile(uint8_t x, uint8_t y, uint8_t ch);

#endif
