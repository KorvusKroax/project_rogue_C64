#include <stdio.h>
#include <c64/memmap.h>

int main(void)
{
    mmap_set(MMAP_NO_BASIC); // BASIC ROM off

    printf("ROGUE FOR C64\n");
    printf("(BASED ON ROGUE 5.4.4)\n");

    mmap_set(MMAP_ROM); // BASIC ROM on
    return 0;
}
