#ifdef DEVMODE
#include "dev.h"

#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "charset.h"
#include "rng.h"
#include "num.h"

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

static int test_ROLL(uint16_t i)
{
    game.seed = 111;

    if (ROLL(0, 6) != 0 || game.seed != 111) return false;

    bool min_seen = false, max_seen = false;
    for (uint16_t j = 0; j < 200; j++) {
        uint16_t r = ROLL(2, 6);
        if (r < 2 || r > 12) return false;
        if (r == 2) min_seen = true;
        if (r == 12) max_seen = true;
    }

    return min_seen && max_seen;
}

static bool spread_in_range(uint16_t n, uint16_t lo, uint16_t hi)
{
    bool min_seen = false, max_seen = false;
    for (uint16_t j = 0; j < 500; j++) {
        uint16_t r = spread(n);
        if (r < lo || r > hi) return false;
        if (r == lo) min_seen = true;
        if (r == hi) max_seen = true;
    }

    return min_seen && max_seen;
}

static int test_spread(uint16_t i)
{
    game.seed = 111;

    return
        spread_in_range(70, 67, 73) &&
        spread_in_range(850, 808, 892);
}

static int test_saturating_arithmetic(uint16_t i)
{
    return
        capped_add(16777200, 100) == 16777215 &&
        capped_sub(10, 30) == 0;
}

static bool text_is(const char *text, uint8_t len, const char *expected)
{
    return len == strlen(expected) && strcmp(text, expected) == 0;
}

static int test_format(uint16_t i)
{
    char b[12];

    return
        text_is(b, format_int(b, 0, 0), "0") &&
        text_is(b, format_int(b, 42, 0), "42") &&
        text_is(b, format_int(b, -1, 0), "-1") &&
        text_is(b, format_int(b, 32767, 0), "32767") &&
        text_is(b, format_int(b, -32768, 0), "-32768") &&
        text_is(b, format_uint(b, 0, 0), "0") &&
        text_is(b, format_uint(b, 1000, 0), "1000") &&
        text_is(b, format_uint(b, 16777215, 0), "16777215") &&
        text_is(b, format_uint(b, 4294967295UL, 0), "4294967295") &&
        text_is(b, format_int(b, 42, 5), "   42") &&
        text_is(b, format_int(b, 42, -5), "42   ") &&
        text_is(b, format_int(b, -42, 5), "  -42") &&
        text_is(b, format_int(b, -42, -5), "-42  ") &&
        text_is(b, format_int(b, 12345, 3), "12345") &&
        text_is(b, format_int(b, 12345, -3), "12345");
}



static const struct DevTest dev_tests[] = {
    { "RNG-RN", test_RN, 1 },
    { "RNG-RND", test_rnd, 1 },
    { "RNG-ROLL", test_ROLL, 1 },
    { "RNG-SPREAD", test_spread, 1 },
    { "NUM-SAT ARITHMETIC", test_saturating_arithmetic, 1 },
    { "NUM-FORMAT", test_format, 1 },
};



#define DEV_TEST_COUNT (sizeof(dev_tests) / sizeof(dev_tests[0]))

bool dev_run_tests(void)
{
    bool all_ok = true;
    uint32_t saved_seed = game.seed;

    for (uint8_t t = 0; t < DEV_TEST_COUNT; t++) {
        const struct DevTest *dt = &dev_tests[t];
        uint16_t fails = 0, first_fail = 0;

        screen_column(1);
        put_text(dt->name);
        screen_column(21);

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
                put_text("OK");
            } else {
                put_uint(dt->count, 0);
                put_text("/");
                put_uint(dt->count, 0);
            }
        } else {
            put_uint(dt->count - fails, 0);
            put_text("/");
            put_uint(dt->count, 0);
            put_text(" FAIL AT ");
            put_uint(first_fail, 0);
            all_ok = false;
        }

        screen_newline();
    }

    game.seed = saved_seed;
    return all_ok;
}

// a 95 jel három sorban
static void put_all_glyphs(uint8_t first_row, uint8_t offset)
{
    for (uint8_t ch = GLYPH_FIRST; ch <= GLYPH_LAST; ch++) {
        uint8_t i = ch - GLYPH_FIRST;
        screen_put(i % SCREEN_W, first_row + i / SCREEN_W, ch + offset);
    }
}

void dev_charset_screen(void)
{
    screen_clear();

    screen_at(2, 0);
    put_text("Normal:");
    put_all_glyphs(1, 0);

    screen_at(2, 5);
    put_text("Inverse:");
    put_all_glyphs(6, 128);

    static const uint8_t map_signs[] = {
        PASSAGE, DOOR, FLOOR, PLAYER, TRAP, STAIRS, GOLD, POTION, SCROLL, WALL_H, WALL_V,
        FOOD, WEAPON, ARMOR, AMULET, RING, STICK, MAGIC, SOLID_ROCK, 'A', 'Z',
    };
    for (uint8_t i = 0; i < sizeof(map_signs); i++) {
        screen_put(i, 10, map_signs[i]);
    }

    static const char *const map_sample[] = {
        "-------+--------    #####",
        "|..*...:...!...|    #",
        "|.@.K..?..)..].+#####",
        "|..^..=../..,..|",
        "|......%.......|",
        "----------------",
    };
    for (uint8_t i = 0; i < 6; i++) {
        screen_at(0, 12 + i);
        put_text(map_sample[i]);
    }

    // a sor vége után nem szabad semminek átcsúsznia a következő sorba
    screen_at(30, 18);
    put_text("Cut at the edge: X");

    // clear_to_line_end teszt
    for (uint8_t x = 0; x < SCREEN_W; x++) {
        screen_put(x, 20, 'x');
        screen_put(x, 21, 'y');
    }
    screen_at(20, 20);
    clear_to_line_end();

    screen_at(2, 24);
    put_text("Press a key");

    // a négy sarok: a KERNAL a jobb alsónál görgetne
    screen_put(0, 0, '1');
    screen_put(SCREEN_W - 1, 0, '2');
    screen_put(0, SCREEN_H - 1, '3');
    screen_put(SCREEN_W - 1, SCREEN_H - 1, '4');
}

#endif
