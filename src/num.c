#include "num.h"

#include <stdbool.h>

#define CAPPED_MAX 16777215UL
#define POWER_COUNT 10

uint16_t dist2(struct Position a, struct Position b)
{
    int8_t dx = a.x - b.x;
    int8_t dy = a.y - b.y;

    return dx * dx + dy * dy;
}

uint32_t capped_add(uint32_t a, uint32_t b)
{
    uint32_t sum = a + b;
    return sum > CAPPED_MAX ? CAPPED_MAX : sum;
}

uint32_t capped_sub(uint32_t a, uint32_t b)
{
    return b > a ? 0 : a - b;
}

static const uint32_t powers_of_ten[POWER_COUNT] = {
    1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1
};

static uint8_t format_number(char *out, uint32_t value, bool negative, int8_t width)
{
    char text[11];
    uint8_t length = 0;
    bool started = false;

    if (negative) {
        text[length++] = '-';
    }

    for (uint8_t power_index = 0; power_index < POWER_COUNT; power_index++) {
        char digit = '0';
        while (value >= powers_of_ten[power_index]) {
            value -= powers_of_ten[power_index];
            digit++;
        }

        if (digit != '0' || power_index == POWER_COUNT - 1) {
            started = true;
        }
        if (started) {
            text[length++] = digit;
        }
    }

    uint8_t field_width = width < 0 ? -width : width;
    if (field_width > FORMAT_WIDTH_MAX) {
        field_width = FORMAT_WIDTH_MAX;
    }

    uint8_t padding = field_width > length ? field_width - length : 0;
    uint8_t out_index = 0;

    if (width > 0) {
        while (padding > 0) {
            out[out_index++] = ' ';
            padding--;
        }
    }
    for (uint8_t i = 0; i < length; i++) {
        out[out_index++] = text[i];
    }
    while (padding > 0) {
        out[out_index++] = ' ';
        padding--;
    }
    out[out_index] = 0;

    return out_index;
}

uint8_t format_int(char *out, int16_t value, int8_t width)
{
    if (value < 0) {
        return format_number(out, -(int32_t)value, true, width);
    }

    return format_number(out, value, false, width);
}

uint8_t format_uint(char *out, uint32_t value, int8_t width)
{
    return format_number(out, value, false, width);
}
