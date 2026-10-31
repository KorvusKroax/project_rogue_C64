#include "num.h"

#include <stdbool.h>

#define CAPPED_MAX 16777215UL
#define POWER_COUNT 10

uint16_t dist2(struct Pos a, struct Pos b)
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
    uint8_t len = 0;
    bool started = false;

    if (negative) {
        text[len++] = '-';
    }

    for (uint8_t p = 0; p < POWER_COUNT; p++) {
        char digit = '0';
        while (value >= powers_of_ten[p]) {
            value -= powers_of_ten[p];
            digit++;
        }

        if (digit != '0' || p == POWER_COUNT - 1) {
            started = true;
        }
        if (started) {
            text[len++] = digit;
        }
    }

    uint8_t field = width < 0 ? -width : width;
    uint8_t padding = field > len ? field - len : 0;
    uint8_t pos = 0;

    if (width > 0) {
        for (; padding > 0; padding--) {
            out[pos++] = ' ';
        }
    }
    for (uint8_t i = 0; i < len; i++) {
        out[pos++] = text[i];
    }
    for (; padding > 0; padding--) {
        out[pos++] = ' ';
    }
    out[pos] = 0;

    return pos;
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
