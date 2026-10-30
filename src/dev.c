#ifdef DEVMODE

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "dev.h"
#include "platform.h"
#include "rng.h"

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



static int test_RN(uint16_t i)
{
    game.seed = 111;

    return
        RN() == 19 &&
        RN() == 14762 &&
        RN() == 22505 &&
        RN() == 57049 &&
        RN() == 24524;
}

static int test_rnd(uint16_t i)
{
    game.seed = 111;

    if (rnd(0) != 0 || game.seed != 111) return false;

    bool v[10] = {false, false, false, false, false, false, false, false, false, false};
    for (uint16_t j = 0; j < 200; j++) {
        uint16_t r = rnd(10);
        if (r >= 10) return false;
        v[r] = true;
    }
    for (uint8_t j = 0; j < 10; j++) {
        if (!v[j]) return false;
    }

    return true;
}



static const struct DevTest dev_tests[] = {
    { "RNG NEXT", test_RN, 1 },
    { "RNG RANDOM", test_rnd, 1 },
};



#define DEV_TEST_COUNT (sizeof(dev_tests) / sizeof(dev_tests[0]))

bool dev_run_tests(void)
{
    bool all_ok = true;
    uint32_t saved_seed = game.seed;

    iocharmap(IOCHM_PETSCII_1);

    for (uint8_t t = 0; t < DEV_TEST_COUNT; t++) {
        const struct DevTest *dt = &dev_tests[t];
        uint16_t fails = 0, first_fail = 0;

        printf("  %s: ", dt->name);

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

    game.seed = saved_seed;
    return all_ok;
}

#endif
