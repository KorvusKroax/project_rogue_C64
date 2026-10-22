#include <stdio.h>
#include "rng.h"
#include "room.h"
#include "platform.h"

int main(void)
{
    clear_screen();

    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, rnd(100));
    }

    draw_room(12, 3, 10, 5);

    return 0;
}
