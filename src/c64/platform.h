#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

extern volatile uint8_t frame_count;    // képkockánként egyet nő (PAL: 50/mp)

void platform_init(void);   // BASIC ki, IRQ/NMI beállítás
void platform_exit(void);   // BASIC be, IRQ/NMI visszaállítás
void wait_frame(void);      // vár a következő képkocka elejéig

#endif
