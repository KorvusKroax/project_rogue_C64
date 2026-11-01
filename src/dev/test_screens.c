#include "dev.h"

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"
#include "../platform/charset.h"
#include "../map_signs.h"

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
        " -------+--------    #####",
        " |..*...:...!...|    #",
        " |.@.K..?..)..].+#####",
        " |..^..=../..,..|",
        " |......%.......|",
        " ----------------",
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

    // a négy sarok: a KERNAL a jobb alsónál görgetne
    screen_put(0, 0, '1');
    screen_put(SCREEN_W - 1, 0, '2');
    screen_put(0, SCREEN_H - 1, '3');
    screen_put(SCREEN_W - 1, SCREEN_H - 1, '4');
}

void dev_keyboard_screen(void)
{
    uint8_t last_code = 0, presses = 0;

    screen_clear();
    screen_at(2, 0);
    put_text("Keyboard matrix");
    screen_at(2, 1);
    put_text("0..7 column, row bits down");
    screen_at(2, 13);
    put_text("Modifiers:");
    screen_at(2, 15);
    put_text("Last press:");
    screen_at(2, 16);
    put_text("Presses:");
    screen_at(2, 24);
    put_text("RETURN to leave, SHIFT: stop reading");

    while (last_code != KEY_RETURN) {
        wait_frame();

        // a SHIFT nyomva tartása alatt a puffer megtelhet
        if (!(key_modifiers & KEY_MOD_SHIFT)) {
            uint8_t code = key_get();
            if (code) {
                last_code = code;
                presses++;
            }
        }

        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
                screen_put(2 + column, 3 + row, (key_matrix[column] >> row) & 1 ? '#' : '.');
            }
        }

        screen_at(13, 13);
        put_text(key_modifiers & KEY_MOD_SHIFT ? "SHIFT " : "      ");
        put_text(key_modifiers & KEY_MOD_CTRL ? "CTRL" : "    ");

        screen_at(14, 15);
        put_uint(last_code, 3);
        screen_at(14, 16);
        put_uint(presses, 3);
    }
}
