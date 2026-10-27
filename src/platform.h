#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_W  40
#define SCREEN_H  25

// indítás és leállás
void platform_init(void);
void platform_exit(void);

// képernyő
void screen_clear(void);
void screen_put(uint8_t x, uint8_t y, uint8_t ch); // ch: ASCII (inverz: +128)

// billentyűzet
uint8_t key_get(void); // ASCII kód, 0 ha nincs leütés

// lemez
bool disk_exists(const char *name);
bool disk_save(const char *name, const void *data, uint16_t size);
bool disk_load(const char *name, void *data, uint16_t size);

// időzítés
void wait_frame(void);

#endif
