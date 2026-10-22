#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t  seed = 111;

uint16_t  RN(void)
{
    seed = seed * 11109 + 13849;
    return (uint16_t)(seed >> 16);
}

int rnd(int n)
{
    return n == 0 ? 0 : RN() % abs(n);
}

int main(void)
{
    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, rnd(100));
    }

    return 0;
}
