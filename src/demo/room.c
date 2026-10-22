#include "room.h"

#define SCREEN ((char*) 0x0400)

void draw_room(int x, int y, int w, int h)
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
