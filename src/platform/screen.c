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

void set_screen_colors(uint8_t border, uint8_t background, uint8_t text)
{
    for (uint8_t i = 0; i < 250; i++) {
        COLOR_RAM[i]       = text;
        COLOR_RAM[i + 250] = text;
        COLOR_RAM[i + 500] = text;
        COLOR_RAM[i + 750] = text;
    }

    vic.color_border = border;
    vic.color_back   = background;
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

void put_screen_character(uint8_t x, uint8_t y, uint8_t character)
{
    SCREEN_RAM[x + y * SCREEN_WIDTH] = character;
}

void set_screen_cursor(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void set_screen_cursor_x(uint8_t x)
{
    cursor_x = x;
}

uint8_t get_screen_cursor_x(void)
{
    return cursor_x;
}

uint8_t get_screen_cursor_y(void)
{
    return cursor_y;
}

// az utolsó sorban marad: nem görget
void move_screen_cursor_to_next_row(void)
{
    cursor_x = 0;
    if (cursor_y < SCREEN_HEIGHT - 1) {
        cursor_y++;
    }
}

// a sor végén levág, nem tör és nem görget
void put_text(const char *text)
{
    while (*text && cursor_x < SCREEN_WIDTH) {
        put_screen_character(cursor_x++, cursor_y, *text++);
    }
}

void put_int(int32_t value, int8_t width)
{
    char text[FORMAT_BUFFER_SIZE];
    format_int(text, value, width);
    put_text(text);
}

void screen_copy_row(uint8_t y, const uint8_t *source)
{
    volatile uint8_t *target = SCREEN_RAM + y * SCREEN_WIDTH;
    for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
        target[x] = source[x];
    }
}

void clear_to_row_end(void)
{
    for (uint8_t x = cursor_x; x < SCREEN_WIDTH; x++) {
        put_screen_character(x, cursor_y, ' ');
    }
}
