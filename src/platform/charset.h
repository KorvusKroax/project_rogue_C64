#ifndef CHARSET_H
#define CHARSET_H

#include <stdint.h>

#define GLYPH_FIRST  32
#define GLYPH_LAST   126

// a 0..127. jel képét a target címre írja (csak tiltott megszakítással hívható)
void charset_load(volatile uint8_t *target);

#endif
