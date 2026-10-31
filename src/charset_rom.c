#include "charset.h"

#include <c64/memmap.h>

// a gyári kisbetűs készlet a ROM-ban
#define CHAR_ROM   ((volatile uint8_t *)0xD800)
#define NOT_IN_ROM 255

// a ROM-ból hiányzó jelek képe, kódsorrendben, jelenként 8 sor
static const uint8_t own_glyphs[] = {
    // '\' (92)
    0b01000000,
    0b01100000,
    0b00110000,
    0b00011000,
    0b00001100,
    0b00000110,
    0b00000010,
    0b00000000,

    // '^' (94)
    0b00011000,
    0b00111100,
    0b01100110,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // '_' (95)
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111110,

    // '`' (96)
    0b00110000,
    0b00011000,
    0b00001100,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // '{' (123)
    0b00001100,
    0b00011000,
    0b00011000,
    0b00110000,
    0b00011000,
    0b00011000,
    0b00001100,
    0b00000000,

    // '|' (124)
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000,

    // '}' (125)
    0b00110000,
    0b00011000,
    0b00011000,
    0b00001100,
    0b00011000,
    0b00011000,
    0b00110000,
    0b00000000,

    // '~' (126)
    0b00000000,
    0b00000000,
    0b01110110,
    0b11011100,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

// az ASCII-jel helye a ROM-ban
static uint8_t rom_index(uint8_t ch)
{
    if (ch == '@') return 0;
    if (ch == '[') return 27;
    if (ch == ']') return 29;
    if (ch >= 'a' && ch <= 'z') return ch - 96;
    if (ch <= 'Z') return ch;
    return NOT_IN_ROM;
}

void charset_load(volatile uint8_t *target)
{
    const uint8_t *own = own_glyphs;

    mmap_set(MMAP_CHAR_ROM);
    for (uint8_t ch = 0; ch < 128; ch++) {
        uint8_t index = rom_index(ch);
        for (uint8_t row = 0; row < 8; row++) {
            uint8_t bits = 0;
            if (ch >= GLYPH_FIRST && ch <= GLYPH_LAST) {
                bits = index == NOT_IN_ROM ? *own++ : CHAR_ROM[index * 8 + row];
            }
            *target++ = bits;
        }
    }
    mmap_set(MMAP_NO_BASIC);
}
