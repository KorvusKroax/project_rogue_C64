#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

extern volatile uint8_t frame_count;    // a raszter-IRQ lépteti, körbefut

void platform_init(void);       // indítás
void platform_exit(void);       // leállás
uint32_t calculate_seed(void);   // seed a hardveres időzítőkből (E13)
void wait_for_next_frame(void);          // időzítés

#endif
