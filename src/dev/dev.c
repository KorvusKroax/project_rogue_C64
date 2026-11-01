#include "dev.h"

#include <stdlib.h>

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"

void dev_fail(const char *msg, const char *file, int line)
{
    // a __FILE__ teljes útvonalából csak a fájlnév kell
    const char *name = file;
    for (const char *p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }

    screen_clear();
    screen_at(0, 0);
    put_text("DEV_CHECK: ");
    put_text(msg);
    screen_at(0, 1);
    put_text(name);
    put_text(":");
    put_uint(line, 0);

    while (!key_get()) {}

    platform_exit();
    exit(1);
}
