#include <stdio.h>
#include <c64/memmap.h>
#include "rng.h"
#include "room.h"
#include "platform.h"

uint16_t ram_test(void)
{
    volatile uint8_t *p = (volatile uint8_t *)0xA000;
    uint16_t bad = 0;
    for (uint16_t i = 0; i < 0x2000; i++) p[i] = (uint8_t)(i * 7 + 3);
    for (uint16_t i = 0; i < 0x2000; i++) if (p[i] != (uint8_t)(i * 7 + 3)) bad++;
    return bad;
}

int main(void)
{
    clear_screen();

    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, rnd(100));
    }

    draw_room(12, 3, 10, 5);

    printf("RAM TEST:\n");
    printf(" BASIC KIKAPCSOLAS ELOTT: %u HIBA\n", ram_test());   // Oscar64: kb. 8000 (a ROM latszik)
    mmap_set(MMAP_NO_BASIC); // BASIC kikapcsolása
    printf(" BASIC KIKAPCSOLAS UTAN:  %u HIBA\n", ram_test());   // mindket forditonal: 0

    mmap_set(MMAP_ROM); // BASIC visszakapcsolása
    return 0;
}
