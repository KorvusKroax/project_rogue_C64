#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SCREEN_W  40
#define SCREEN_H  25

void screen_init(void);     // csak tiltott megszakítással hívható

void screen_color(uint8_t br, uint8_t bg, uint8_t tx);
void screen_clear(void);

void screen_put(uint8_t x, uint8_t y, uint8_t ch);      // ch: ASCII (inverz: +128)
void screen_at(uint8_t x, uint8_t y);                   // innen ír a put_text és a put_uint
void screen_column(uint8_t x);                          // ugyanebben a sorban az x. oszlopba
void screen_newline(void);                              // a következő sor elejére
void put_text(const char *text);
void put_uint(uint32_t value, int8_t width);            // width: mint a format_uint-nél
void screen_copy_row(uint8_t y, const uint8_t *source); // SCREEN_W jel az y. sorba
void clear_to_line_end(void);                           // a screen_at helyétől a sor végéig

#endif
