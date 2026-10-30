#include "num.h"

uint16_t dist2(struct Pos a, struct Pos b)
{
    int8_t dx = a.x - b.x;
    int8_t dy = a.y - b.y;

    return dx * dx + dy * dy;
}
