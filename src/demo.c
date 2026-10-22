#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define SCREEN ((char*) 0x0400)

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

void draw_room(int x, int y, int w, int h) // w and h means with walls
{
    int index = x + y * 40;
    for (int i = 0; i < w; i++) { // 67 = "-"
        SCREEN[index+i] = 67;
        SCREEN[index+i+(h-1)*40] = 67;
    }
    for (int i = 1; i < h-1; i++) { // 66 = "|"
        SCREEN[index+i*40] = 66;
        SCREEN[index+i*40+(w-1)] = 66;
    }
}

int main(void)
{
    clrscr();

    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, rnd(100));
    }

    draw_room(12, 3, 10, 5);

    return 0;
}
