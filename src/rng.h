#ifndef RNG_H
#define RNG_H

#include <stdint.h>

uint16_t RN();
uint16_t rnd(uint16_t n);
uint16_t ROLL(uint16_t count, uint16_t sides);
uint16_t spread(uint16_t n);

#endif
