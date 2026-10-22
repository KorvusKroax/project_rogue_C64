#include <stdlib.h>
#include "rng.h"
#include "platform.h"

ASM_USED uint32_t seed = 111;

uint16_t RN(void)
{
    seed = seed * 11109 + 13849;
    return (uint16_t)(seed >> 16);
}

int rnd(int n)
{
    return n == 0 ? 0 : RN_asm() % abs(n);
}
