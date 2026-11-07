#ifndef CHARSET_H
#define CHARSET_H

#include <stdint.h>

#define GLYPH_FIRST  ' '                        // a megjeleníthető ASCII-jelek első és utolsó kódja
#define GLYPH_LAST   '~'

#define GLYPH_COUNT     128                     // a készlet alsó fele; a felső fele ugyanez inverzben
#define GLYPH_BYTES     8                       // jelenként 8 sor
#define INVERSE_OFFSET  GLYPH_COUNT             // a jel inverz változatának kódja ennyivel nagyobb

// a 0..127. jel képét a target címre írja (csak tiltott megszakítással hívható)
void charset_load(volatile uint8_t *target);

#endif
