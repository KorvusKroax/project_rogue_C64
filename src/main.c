#include <stdio.h>

#include "platform.h"
#include "state.h"

int main(void)
{
    platform_init();

    screen_clear();

#ifdef DEVMODE

    printf(" !!! DEVMODE !!!\n\n");

#endif

    printf("ROGUE FOR C64\n");
    printf("(BASED ON ROGUE 5.4.4)\n");

    screen_put(10, 5, 0);

    for (uint8_t i = 0; i < 16; i++) {
        for (uint8_t j = 0; j < 50; j++) {

            uint8_t keyCode = key_get();
            if (keyCode) {
                printf("KEYCODE: %i\n", keyCode);
            }

            wait_frame();
        }
        (*(volatile uint8_t *)0xd020)++; // keretszín
    }

    platform_exit();
    return 0;
}
