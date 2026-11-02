#include "tests.h"

#include <stdbool.h>

#include "../rng.h"
#include "../state.h"

int test_RN(uint16_t i)
{
    game.seed = 111;

    return
        RN() == 19 &&
        RN() == 14762 &&
        RN() == 22505 &&
        RN() == 57049 &&
        RN() == 24524;
}

int test_rnd(uint16_t i)
{
    game.seed = 111;

    if (rnd(0) != 0 || game.seed != 111) return false;

    bool seen[10] = {false, false, false, false, false, false, false, false, false, false};
    for (uint16_t j = 0; j < 200; j++) {
        uint16_t result = rnd(10);
        if (result >= 10) return false;
        seen[result] = true;
    }
    for (uint8_t j = 0; j < 10; j++) {
        if (!seen[j]) return false;
    }

    return true;
}

int test_ROLL(uint16_t i)
{
    game.seed = 111;

    if (ROLL(0, 6) != 0 || game.seed != 111) return false;

    bool min_seen = false, max_seen = false;
    for (uint16_t j = 0; j < 200; j++) {
        uint16_t result = ROLL(2, 6);
        if (result < 2 || result > 12) return false;
        if (result == 2) min_seen = true;
        if (result == 12) max_seen = true;
    }

    return min_seen && max_seen;
}

static bool spread_in_range(uint16_t n, uint16_t lo, uint16_t hi)
{
    bool min_seen = false, max_seen = false;
    for (uint16_t j = 0; j < 500; j++) {
        uint16_t result = spread(n);
        if (result < lo || result > hi) return false;
        if (result == lo) min_seen = true;
        if (result == hi) max_seen = true;
    }

    return min_seen && max_seen;
}

int test_spread(uint16_t i)
{
    game.seed = 111;

    return
        spread_in_range(70, 67, 73) &&
        spread_in_range(850, 808, 892);
}
