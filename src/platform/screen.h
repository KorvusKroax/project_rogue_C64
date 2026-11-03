#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SCREEN_WIDTH   40
#define SCREEN_HEIGHT  25

void screen_init(void);     // csak tiltott megszakítással hívható

void set_screen_colors(uint8_t border, uint8_t background, uint8_t text);
void screen_clear(void);

void put_screen_character(uint8_t x, uint8_t y, uint8_t character);     // character: ASCII (inverz: +128)
void set_screen_cursor(uint8_t x, uint8_t y);                           // innen ír a put_text és a put_uint
void set_screen_cursor_x(uint8_t x);                                    // ugyanebben a sorban az x. oszlopba
uint8_t get_screen_cursor_x(void);
uint8_t get_screen_cursor_y(void);
void move_screen_cursor_to_next_row(void);                              // a következő sor elejére
void put_text(const char *text);
void put_uint(uint32_t value, int8_t width);                            // width: mint a format_uint-nél
void screen_copy_row(uint8_t y, const uint8_t *source);                 // SCREEN_WIDTH jel az y. sorba
void clear_to_row_end(void);                                            // a kurzortól a sor végéig

#endif
