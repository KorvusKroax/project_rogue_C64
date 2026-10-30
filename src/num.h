#ifndef NUM_H
#define NUM_H

#include <stdint.h>

struct Pos {
    uint8_t x, y;
};

uint16_t dist2(struct Pos a, struct Pos b);

#endif
