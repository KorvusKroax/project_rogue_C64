#ifndef NUM_H
#define NUM_H

#include <stdint.h>

#define FORMAT_DIGITS_MAX   10                          // a 32 bites szám legfeljebb 10 jegyű
#define FORMAT_WIDTH_MAX    (FORMAT_DIGITS_MAX + 1)     // a jegyek az előjellel; ennél szélesebb mezőt nem ír a format_int
#define FORMAT_BUFFER_SIZE  (FORMAT_WIDTH_MAX + 1)

struct Position {
    uint8_t x, y;
};

uint16_t dist2(struct Position a, struct Position b);
uint32_t capped_add(uint32_t a, uint32_t b);
uint32_t capped_sub(uint32_t a, uint32_t b);
uint8_t format_int(char *out, int32_t number, int8_t width);

#endif
