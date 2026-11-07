#include "tests.h"

#include <stdbool.h>

#include "../view.h"

struct ViewCase {
    uint8_t view_window_x, x, expected;
};

// rövid nevek a táblához
#define LAST_COLUMN   (MAP_COLUMNS - 1)
#define WIDTH         VIEW_WINDOW_WIDTH
#define EDGE          VIEW_WINDOW_EDGE_DISTANCE
#define X_MAX         VIEW_WINDOW_X_MAX
#define X_MIDDLE      (VIEW_WINDOW_X_MAX / 2)

static const struct ViewCase view_cases[] = {
    // a széleken nem lóg ki, és a holt zónán belül marad
    { 0, 0, 0 }, { X_MAX, LAST_COLUMN, X_MAX }, { 0, WIDTH - 1 - EDGE, 0 },
    // teleport a térkép két szélére
    { X_MAX, 0, 0 }, { 0, LAST_COLUMN, X_MAX },

// a görgetés eseteihez a nézőablaknak a holt zónáknál többet kell tudnia mozogni
#if X_MAX > 2 * EDGE
    // a holt zónán belül marad
    { X_MIDDLE, X_MIDDLE + EDGE, X_MIDDLE }, { X_MIDDLE, X_MIDDLE + WIDTH - 1 - EDGE, X_MIDDLE }, { X_MAX, X_MAX + EDGE, X_MAX },
    // oszloponként görget
    { 0, WIDTH - EDGE, 1 }, { X_MIDDLE, X_MIDDLE + WIDTH - EDGE, X_MIDDLE + 1 }, { X_MIDDLE, X_MIDDLE + EDGE - 1, X_MIDDLE - 1 }, { X_MAX, X_MAX + EDGE - 1, X_MAX - 1 },
    // a széleken nem lóg ki
    { 1, EDGE, 0 }, { X_MAX - 1, X_MAX - 1 + WIDTH - EDGE, X_MAX },
    // teleport a nézőablakon kívülre
    { X_MAX, X_MAX - 1, X_MAX - 1 - EDGE }, { 0, WIDTH + EDGE, 2 * EDGE + 1 },
#endif
};

int test_view_follow(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(view_cases) / sizeof(view_cases[0]); c++) {
        const struct ViewCase *view_case = &view_cases[c];
        if (calculate_view_window_x_after_step(view_case->x, view_case->view_window_x) != view_case->expected) return false;
    }

    return true;
}
