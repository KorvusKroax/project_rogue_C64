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
    put_uint(line, 0);

    wait_for_key();

    platform_exit();
    exit(1);
}
