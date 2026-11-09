#ifndef RNG_H
#define RNG_H

#include <stdint.h>

void rng_init(void);                    // a szorzótáblák feltöltése; az első RN előtt kell hívni
uint16_t RN(void);
uint16_t rnd(uint16_t n);
uint16_t ROLL(uint16_t count, uint16_t sides);
uint16_t spread(uint16_t n);

#endif
