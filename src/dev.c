#ifdef DEVMODE

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "dev.h"
#include "platform.h"

void dev_fail(const char *msg, const char *file, int line)
{
    // a __FILE__ teljes útvonalából csak a fájlnév kell
    const char *name = file;
    for (const char *p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }

    iocharmap(IOCHM_PETSCII_1);
    printf("\nDEV\244CHECK: %s\n%s:%d\n", msg, name, line);
    platform_exit();
    exit(1);
}

#endif
