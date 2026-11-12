#include "dev.h"

#include <stdlib.h>

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"

void dev_fail(const char *message, const char *file, int line)
{
    // a __FILE__ teljes útvonalából csak a fájlnév kell
    const char *name = file;
    for (const char *p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }

    screen_clear();
    set_screen_cursor(0, 0);
    put_text("DEV_CHECK: ");
    put_text(message);
    set_screen_cursor(0, 1);
    put_text(name);
    put_text(":");
    put_int(line, 0);

    wait_for_key();

    platform_exit();
    exit(1);
}

bool convert_text_to_number(const char *text, uint32_t max_value, uint32_t *number)
{
    if (*text == 0) return false;

    uint32_t value = 0;
    while (*text) {
        if (*text < '0' || *text > '9') return false;

        // value * 10 + digit <= max_value, túlcsordulás nélkül ellenőrizve
        uint8_t digit = *text - '0';
        if (digit > max_value || value > (max_value - digit) / 10) return false;

        value = value * 10 + digit;
        text++;
    }

    *number = value;
    return true;
}
