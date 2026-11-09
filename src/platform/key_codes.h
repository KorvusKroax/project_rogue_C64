#ifndef KEY_CODES_H
#define KEY_CODES_H

#include <stdint.h>

// a mátrixhely (oszlop * KEY_ROWS + sor) kódja a módosítókkal (KEY_MODIFIER_*), 0 ha nincs; a billentyűzet-IRQ hívja
uint8_t get_key_code(uint8_t position, uint8_t modifiers);

#endif
