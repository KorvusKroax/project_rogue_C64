#include "tests.h"

#include <stdbool.h>

#include "../message.h"

#define TWENTY_CHAR     "aaaaaaaaaaaaaaaaaaaa"
#define THIRTY_TWO_CHAR "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define FORTY_CHAR      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

struct LengthCase {
    const char *text;
    uint8_t expected;
};

static const struct LengthCase row_cases[] = {
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

static const struct LengthCase bar_cases[] = {
    { "short text", 10 },
    { FORTY_CHAR, 40 },

    // a második sorban a --More-- helye is megvan
    { FORTY_CHAR " " THIRTY_TWO_CHAR, 73 },
    { FORTY_CHAR " " THIRTY_TWO_CHAR "a", 73 },
    { FORTY_CHAR " " TWENTY_CHAR " " TWENTY_CHAR, 61 },

    // ami két sornál több, a következő oldalra marad
    { FORTY_CHAR " " FORTY_CHAR " " FORTY_CHAR, 73 },
};

int test_message_chars_fitting_row(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(row_cases) / sizeof(row_cases[0]); c++) {
        const struct LengthCase *row_case = &row_cases[c];
        if (calculate_message_chars_fitting_row(row_case->text, 40) != row_case->expected) return false;
    }

    return true;
}

int test_message_chars_fitting_bar(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(bar_cases) / sizeof(bar_cases[0]); c++) {
        const struct LengthCase *bar_case = &bar_cases[c];
        if (calculate_message_chars_fitting_bar(bar_case->text) != bar_case->expected) return false;
    }

    return true;
}
