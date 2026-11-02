#include "tests.h"

#include <stdbool.h>

#include "../message.h"

#define FORTY_CHAR "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

struct LineCase {
    const char *text;
    uint8_t expected;
};

static const struct LineCase line_cases[] = {
    { "", 0 },
    { "short text", 10 },

    // pontosan kitölti a sort
    { FORTY_CHAR, 40 },
    { FORTY_CHAR " x", 40 },

    // az utolsó kiférő szóköznél törik
    { "you feel a wrenching sensation in your gut", 38 },
    { "a " FORTY_CHAR, 1 },

    // a sornál hosszabb szó keményen törik
    { FORTY_CHAR "a", 40 },
};

int test_message_line_length(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(line_cases) / sizeof(line_cases[0]); c++) {
        const struct LineCase *l = &line_cases[c];
        if (message_line_length(l->text) != l->expected) return false;
    }

    return true;
}
