#include "screen.h"

#include <c64/vic.h>

#include "charset.h"
#include "../num.h"

#define SCREEN_RAM   ((volatile uint8_t *)0xC000)
#define CHARSET_RAM  ((volatile uint8_t *)0xC800)
#define COLOR_RAM    ((volatile uint8_t *)0xD800)

static uint8_t cursor_x, cursor_y;

void screen_init(void)
{
    // a felső fél az alsó negáltja
    charset_load(CHARSET_RAM);
    for (uint16_t i = 0; i < 1024; i++) {
        CHARSET_RAM[i + 1024] = ~CHARSET_RAM[i];
    }

    vic_setmode(VICM_TEXT, (const char *)SCREEN_RAM, (const char *)CHARSET_RAM);
}

void screen_color(uint8_t br, uint8_t bg, uint8_t tx)
{
    for (uint8_t i = 0; i < 250; i++) {
        COLOR_RAM[i]       = tx;
        COLOR_RAM[i + 250] = tx;
        COLOR_RAM[i + 500] = tx;
        COLOR_RAM[i + 750] = tx;
    }

    vic.color_border = br;
    vic.color_back   = bg;
}

void screen_clear(void)
{
    for (uint8_t i = 0; i < 250; i++) {
        SCREEN_RAM[i]       = ' ';
        SCREEN_RAM[i + 250] = ' ';
        SCREEN_RAM[i + 500] = ' ';
        SCREEN_RAM[i + 750] = ' ';
    }
}

void screen_put(uint8_t x, uint8_t y, uint8_t ch)
{
    SCREEN_RAM[x + y * SCREEN_W] = ch;
}

void screen_at(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void screen_column(uint8_t x)
{
    cursor_x = x;
}

// az utolsó sorban marad: nem görget
void screen_newline(void)
{
    cursor_x = 0;
    if (cursor_y < SCREEN_H - 1) {
        cursor_y++;
    }
}

// a sor végén levág, nem tör és nem görget
void put_text(const char *text)
{
    while (*text && cursor_x < SCREEN_W) {
        screen_put(cursor_x++, cursor_y, *text++);
    }
}

void put_uint(uint32_t value, int8_t width)
{
    char text[12];
    format_uint(text, value, width);
    put_text(text);
}

void screen_copy_row(uint8_t y, const uint8_t *source)
{
    volatile uint8_t *target = SCREEN_RAM + y * SCREEN_W;
    for (uint8_t x = 0; x < SCREEN_W; x++) {
        target[x] = source[x];
    }
}

void clear_to_line_end(void)
{
    for (uint8_t x = cursor_x; x < SCREEN_W; x++) {
        screen_put(x, cursor_y, ' ');
    }
}
