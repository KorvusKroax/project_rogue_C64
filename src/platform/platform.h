#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#define RASTER_INTERRUPT_ROW  250       // a raszter-IRQ sora, a látható rész alatt: itt lép a frame_count

extern volatile uint8_t frame_count;    // a raszter-IRQ lépteti, körbefut

void platform_init(void);       // indítás
uint32_t calculate_seed(void);   // seed a hardveres időzítőkből (E13)
void wait_for_next_frame(void);          // időzítés
uint16_t get_frame_count_16(void);       // képkockaszámláló 16 biten (a jiffy órából, amit a raszter-IRQ képkockánként léptet); hosszabb mérésekhez, körbefut

// a szöveg a destination-be, legfeljebb max_length karakter és a lezáró nulla, így a destination legalább max_length + 1 bájtos;
// bármely címről olvas, a ROM-ok és az I/O alól is, mert a másolás idejére mindet kilapozza, a megszakítást pedig letiltja;
// a destination-be írt lezáró nullára mutat
char *copy_text(char *destination, const char *text, uint8_t max_length);
// mint a copy_text, a table index. elemével; a tábla is bármely címen lehet
char *copy_text_from_table(char *destination, const char *const *table, uint8_t index, uint8_t max_length);

#endif
