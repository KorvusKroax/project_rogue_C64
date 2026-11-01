#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../num.h"

int test_saturating_arithmetic(uint16_t i)
{
    return
        capped_add(16777200, 100) == 16777215 &&
        capped_sub(10, 30) == 0;
}

static bool text_is(const char *text, uint8_t len, const char *expected)
{
    return len == strlen(expected) && strcmp(text, expected) == 0;
}

int test_format(uint16_t i)
{
    char b[12];

    return
        text_is(b, format_int(b, 0, 0), "0") &&
        text_is(b, format_int(b, 42, 0), "42") &&
        text_is(b, format_int(b, -1, 0), "-1") &&
        text_is(b, format_int(b, 32767, 0), "32767") &&
        text_is(b, format_int(b, -32768, 0), "-32768") &&
        text_is(b, format_uint(b, 0, 0), "0") &&
        text_is(b, format_uint(b, 1000, 0), "1000") &&
        text_is(b, format_uint(b, 16777215, 0), "16777215") &&
        text_is(b, format_uint(b, 4294967295UL, 0), "4294967295") &&
        text_is(b, format_int(b, 42, 5), "   42") &&
        text_is(b, format_int(b, 42, -5), "42   ") &&
        text_is(b, format_int(b, -42, 5), "  -42") &&
        text_is(b, format_int(b, -42, -5), "-42  ") &&
        text_is(b, format_int(b, 12345, 3), "12345") &&
        text_is(b, format_int(b, 12345, -3), "12345");
}
