#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SCREEN_WIDTH   40
#define SCREEN_HEIGHT  25

void screen_init(void);     // csak tiltott megszakítással hívható

void set_screen_colors(uint8_t border, uint8_t background, uint8_t text);
uint8_t get_border_color(void);
void set_border_color(uint8_t color);
void screen_clear(void);

void put_screen_character(uint8_t x, uint8_t y, uint8_t character);     // character: ASCII (inverz: +128)
void set_screen_cursor(uint8_t x, uint8_t y);                           // innen ír a put_text és a put_int
void set_screen_cursor_x(uint8_t x);                                    // ugyanebben a sorban az x. oszlopba
uint8_t get_screen_cursor_x(void);
uint8_t get_screen_cursor_y(void);
void move_screen_cursor_to_next_row(void);                              // a következő sor elejére; az utolsó sorból egy sorral feljebb görget
void put_text(const char *text);
void put_int(int32_t value, int8_t width);                              // width: mint a format_int-nél
void screen_copy_row(uint8_t y, const uint8_t *source, uint8_t length); // length jel az y. sor elejére, a sor többi része törlődik
void clear_to_row_end(void);                                            // a kurzortól a sor végéig

#endif
