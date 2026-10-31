#ifndef NUM_H
#define NUM_H

#include <stdint.h>

struct Pos {
    uint8_t x, y;
};

uint16_t dist2(struct Pos a, struct Pos b);
uint32_t capped_add(uint32_t a, uint32_t b);
uint32_t capped_sub(uint32_t a, uint32_t b);
uint8_t format_int(char *out, int16_t value, int8_t width);
uint8_t format_uint(char *out, uint32_t value, int8_t width);

#endif
