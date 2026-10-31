#include "num.h"

#define CAPPED_MAX 16777215UL

uint16_t dist2(struct Pos a, struct Pos b)
{
    int8_t dx = a.x - b.x;
    int8_t dy = a.y - b.y;

    return dx * dx + dy * dy;
}

uint32_t capped_add(uint32_t a, uint32_t b)
{
    uint32_t sum = a + b;
    return sum > CAPPED_MAX ? CAPPED_MAX : sum;
}

uint32_t capped_sub(uint32_t a, uint32_t b)
{
    return b > a ? 0 : a - b;
}
