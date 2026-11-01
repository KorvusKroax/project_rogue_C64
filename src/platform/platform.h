#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

void platform_init(void);       // indítás
void platform_exit(void);       // leállás
uint32_t platform_seed(void);   // seed a hardveres időzítőkből (E13)
void wait_frame(void);          // időzítés

#endif
