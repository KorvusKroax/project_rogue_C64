#include "tests.h"

#include <stdbool.h>

#include "../view.h"

struct ViewCase {
    uint8_t view_window_x, x, expected;
};

static const struct ViewCase view_cases[] = {
    // a holt zónán belül marad
    { 0, 31, 0 }, { 20, 28, 20 }, { 20, 51, 20 }, { 40, 48, 40 },
    // oszloponként görget
    { 0, 32, 1 }, { 20, 52, 21 }, { 20, 27, 19 }, { 40, 47, 39 },
    // a széleken nem lóg ki
    { 1, 8, 0 }, { 39, 71, 40 }, { 0, 0, 0 }, { 40, 79, 40 },
    // teleport a nézőablakon kívülre
    { 40, 30, 22 }, { 40, 0, 0 }, { 0, 70, 39 }, { 0, 79, 40 },
};

int test_view_follow(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(view_cases) / sizeof(view_cases[0]); c++) {
        const struct ViewCase *view_case = &view_cases[c];
        if (calculate_view_window_x_after_step(view_case->x, view_case->view_window_x) != view_case->expected) return false;
    }

    return true;
}
