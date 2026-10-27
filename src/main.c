#include <stdio.h>

#include "c64/platform.h"
#include "c64/memory.h"
#include "state.h"

int main(void)
{
    platform_init();

    printf("ROGUE FOR C64\n");
    printf("(BASED ON ROGUE 5.4.4)\n");

    for (uint8_t i = 0; i < 16; i++) {
        for (uint8_t j = 0; j < 50; j++) {
            wait_frame();
        }
        (*(volatile uint8_t *)0xd020)++;    // keretszín
    }

    platform_exit();
    return 0;
}
