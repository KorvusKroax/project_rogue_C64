#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

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

struct TextCase {
    const char *text, *expected;
};

static const struct TextCase text_cases[] = {
    { "", "" },
    { "you miss the bat", "You miss the bat." },

    // már van a végén írásjel
    { "wait! That's a xeroc!", "Wait! That's a xeroc!" },
    { "really quit?", "Really quit?" },
    { "the end.", "The end." },

    // nem betűvel kezdődik
    { "'x' is not a valid item", "'x' is not a valid item." },

    // tárgylista-sor
    { "a) a mace", "a) a mace" },
};

struct FormatCase {
    const char *text, *first_text, *second_text;
    int16_t number;
    const char *expected;
};

static const struct FormatCase format_cases[] = {
    { "\1 hits you", "the bat", NULL, 0, "The bat hits you." },
    { "you find \3 gold pieces", NULL, NULL, 25, "You find 25 gold pieces." },
    { "\1 \2 \3", "a", "b", -3, "A b -3." },

    // a helyőrzőből lett tárgylista-sor
    { "\1) \2", "c", "a mace", 0, "c) a mace" },
};

// a fordító a literálokat ASCII-ban hagyja, nem alakítja PETSCII-re
static bool are_literals_ascii(void)
{
    static const char sample[] = "aA";
    return sample[0] == 0x61 && sample[1] == 0x41;
}

int test_prepare_message_text(uint16_t i)
{
    if (!are_literals_ascii()) return false;

    for (uint8_t c = 0; c < sizeof(text_cases) / sizeof(text_cases[0]); c++) {
        const struct TextCase *text_case = &text_cases[c];
        if (strcmp(prepare_message_text(text_case->text, NULL, NULL, 0, true), text_case->expected) != 0) return false;
    }

    for (uint8_t c = 0; c < sizeof(format_cases) / sizeof(format_cases[0]); c++) {
        const struct FormatCase *format_case = &format_cases[c];
        const char *prepared = prepare_message_text(format_case->text, format_case->first_text, format_case->second_text, format_case->number, true);
        if (strcmp(prepared, format_case->expected) != 0) return false;
    }

    // a kérdés nem kap záró pontot
    return strcmp(prepare_message_text("file name: ", NULL, NULL, 0, false), "File name: ") == 0;
}

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
