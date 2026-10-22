#ifndef RNG_H
#define RNG_H

#include <stdint.h>

extern uint32_t seed;           // az RN állapota (a platformfüggő RN_asm is ezt írja)

uint16_t RN(void);              // C-ben, a spec 0.2 képlete szerint
int rnd(int n);                 // 0..|n|-1, az RN_asm-et használja

#endif
