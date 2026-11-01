#include "tests.h"

#include <stdbool.h>

#include "../view.h"

struct ViewCase {
    uint8_t view_x, x, expected;
};

static const struct ViewCase view_cases[] = {
    { 0, 31, 0 }, { 20, 28, 20 }, { 20, 51, 20 }, { 40, 48, 40 },   // a holt zónán belül marad
    { 0, 32, 12 }, { 20, 52, 32 }, { 20, 27, 7 }, { 40, 47, 27 },   // görget
    { 12, 19, 0 }, { 32, 72, 40 }, { 0, 0, 0 }, { 40, 79, 40 },     // a széleken nem lóg ki
    { 40, 30, 10 }, { 40, 0, 0 }, { 0, 70, 40 }, { 0, 79, 40 },     // teleport a nézőablakon kívülre
};

int test_view_follow(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(view_cases) / sizeof(view_cases[0]); c++) {
        const struct ViewCase *v = &view_cases[c];
        if (view_x_after_step(v->x, v->view_x) != v->expected) return false;
    }

    return true;
}
