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

// egy önteszt: a függvény az i. próbát futtatja, igaz = sikerült
// int, nem bool: az Oscar64 1.32.273 a függvénymutatón át hívott
// 8 bites visszatérési értéket rossz helyről olvassa
typedef int (*DevTestFn)(uint16_t i);

struct DevTest {
    const char *name;
    DevTestFn   fn;
    uint16_t    count; // hány próba (1 = egyszeri teszt)
};

static int test_uint16_wrap(uint16_t i)
{
    uint16_t a = 65535;
    a++;
    return a == 0;
}

static int test_sample(uint16_t i)
{
    return (i & 1) == (i % 2);
}

// az öntesztek listája: újat ide kell felvenni
static const struct DevTest dev_tests[] = {
    { "UINT16 WRAP", test_uint16_wrap, 1    },
    { "SAMPLE",      test_sample,      1000 },
};

#define DEV_TEST_COUNT  (sizeof(dev_tests) / sizeof(dev_tests[0]))

bool dev_run_tests(void)
{
    bool all_ok = true;

    iocharmap(IOCHM_PETSCII_1);

    for (uint8_t t = 0; t < DEV_TEST_COUNT; t++) {
        const struct DevTest *dt = &dev_tests[t];
        uint16_t fails = 0, first_fail = 0;

        printf("%s: ", dt->name);

        for (uint16_t i = 0; i < dt->count; i++) {
            if (!dt->fn(i)) {
                if (fails == 0) {
                    first_fail = i;
                }
                fails++;
            }
        }

        if (fails == 0) {
            if (dt->count == 1) {
                printf("OK\n");
            } else {
                printf("%u/%u\n", dt->count, dt->count);
            }
        } else {
            printf("%u/%u FAIL AT %u\n", dt->count - fails, dt->count, first_fail);
            all_ok = false;
        }
    }

    return all_ok;
}

#endif
