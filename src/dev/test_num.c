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

static bool text_is(const char *text, uint8_t length, const char *expected)
{
    return length == strlen(expected) && strcmp(text, expected) == 0;
}

int test_format(uint16_t i)
{
    char buffer[FORMAT_BUFFER_SIZE];

    return
        text_is(buffer, format_int(buffer, 0, 0), "0") &&
        text_is(buffer, format_int(buffer, 42, 0), "42") &&
        text_is(buffer, format_int(buffer, -1, 0), "-1") &&
        text_is(buffer, format_int(buffer, 32767, 0), "32767") &&
        text_is(buffer, format_int(buffer, -32768, 0), "-32768") &&
        text_is(buffer, format_uint(buffer, 0, 0), "0") &&
        text_is(buffer, format_uint(buffer, 1000, 0), "1000") &&
        text_is(buffer, format_uint(buffer, 16777215, 0), "16777215") &&
        text_is(buffer, format_uint(buffer, 4294967295UL, 0), "4294967295") &&
        text_is(buffer, format_int(buffer, 42, 5), "   42") &&
        text_is(buffer, format_int(buffer, 42, -5), "42   ") &&
        text_is(buffer, format_int(buffer, -42, 5), "  -42") &&
        text_is(buffer, format_int(buffer, -42, -5), "-42  ") &&
        text_is(buffer, format_int(buffer, 12345, 3), "12345") &&
        text_is(buffer, format_int(buffer, 12345, -3), "12345") &&
        text_is(buffer, format_int(buffer, 42, 100), "         42");
}
