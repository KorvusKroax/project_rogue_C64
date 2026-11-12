#include "tests.h"

#include <stdbool.h>
#include <string.h>

#include "../num.h"
#include "dev.h"

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
        text_is(buffer, format_int(buffer, 1000, 0), "1000") &&
        text_is(buffer, format_int(buffer, 16777215, 0), "16777215") &&
        text_is(buffer, format_int(buffer, 2147483647L, 0), "2147483647") &&
        text_is(buffer, format_int(buffer, -2147483647L - 1, 0), "-2147483648") &&
        text_is(buffer, format_int(buffer, 42, 5), "   42") &&
        text_is(buffer, format_int(buffer, 42, -5), "42   ") &&
        text_is(buffer, format_int(buffer, -42, 5), "  -42") &&
        text_is(buffer, format_int(buffer, -42, -5), "-42  ") &&
        text_is(buffer, format_int(buffer, 12345, 3), "12345") &&
        text_is(buffer, format_int(buffer, 12345, -3), "12345") &&
        text_is(buffer, format_int(buffer, 42, 100), "         42");
}

// sikeres átalakításnál a várt érték jön ki
static bool number_is(const char *text, uint32_t max_value, uint32_t expected)
{
    uint32_t number = 0;
    return convert_text_to_number(text, max_value, &number) && number == expected;
}

// hibás szövegnél hamis, és a szám nem változik
static bool number_rejected(const char *text, uint32_t max_value)
{
    uint32_t number = 12345;
    return !convert_text_to_number(text, max_value, &number) && number == 12345;
}

int test_convert_text_to_number(uint16_t i)
{
    return
        number_is("0", UINT32_MAX, 0) &&
        number_is("42", UINT32_MAX, 42) &&
        number_is("007", UINT32_MAX, 7) &&
        number_is("4294967295", UINT32_MAX, 4294967295UL) &&
        number_is("255", 255, 255) &&
        number_is("5", 5, 5) &&
        number_rejected("4294967296", UINT32_MAX) &&
        number_rejected("10000000000", UINT32_MAX) &&
        number_rejected("256", 255) &&
        number_rejected("6", 5) &&
        number_rejected("", UINT32_MAX) &&
        number_rejected("12a", UINT32_MAX) &&
        number_rejected("-1", UINT32_MAX) &&
        number_rejected(" 1", UINT32_MAX);
}
