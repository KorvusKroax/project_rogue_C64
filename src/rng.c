#include "rng.h"

#include "state.h"

uint16_t RN(void)
{
    game.seed = game.seed * 11109 + 13849;
    return game.seed >> 16;
}

uint16_t rnd(uint16_t n)
{
    return n == 0 ? 0 : RN() % n;
}

uint16_t ROLL(uint16_t count, uint16_t sides)
{
    uint16_t sum = 0;
    for (uint16_t i = 0; i < count; i++) {
        sum += rnd(sides) + 1;
    }

    return sum;
}

uint16_t spread(uint16_t n)
{
    return n - n / 20 + rnd(n / 10);
}
