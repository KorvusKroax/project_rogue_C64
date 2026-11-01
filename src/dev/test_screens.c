#include "dev.h"

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"
#include "../platform/charset.h"
#include "../map_signs.h"
#include "../state.h"
#include "../view.h"

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

// az 1-2. sor az oszlop száma, a többi betűje tízesenként vált
static uint8_t view_pattern(uint8_t x, uint8_t y)
{
    if (y == 1) return '0' + x / 10;
    if (y == 2) return '0' + x % 10;
    return x % 10 == 0 ? '|' : 'a' + x / 10;
}

#define VIEW_TEST_ROW  12

// a jel a mintát visszaállítva lép, a széleken megáll
static uint8_t view_step(uint8_t x, int8_t step)
{
    int8_t target = x + step;
    if (target < 0) target = 0;
    if (target > NUMCOLS - 1) target = NUMCOLS - 1;

    draw_tile(x, VIEW_TEST_ROW, view_pattern(x, VIEW_TEST_ROW));
    draw_tile(target, VIEW_TEST_ROW, PLAYER);
    follow_view(target);

    return target;
}

#define VIEW_MEASURE_LIMIT  100     // görgetésenként legfeljebb 2 képkocka

// 50 teljes újrarajzolás két távoli nézet között, a képkockák száma
static uint8_t view_measure(void)
{
    uint8_t saved_view_x = game.view_x;
    uint8_t start = frame_count;

    for (uint8_t i = 0; i < 50; i++) {
        game.view_x = i & 1 ? 0 : 40;
        redraw_map();
    }

    uint8_t frames = frame_count - start;
    game.view_x = saved_view_x;
    redraw_map();
    return frames;
}

void dev_view_screen(void)
{
    uint8_t code = 0, x = 20, frames = 0;

    screen_clear();
    screen_at(0, 23);
    put_text("A,D: step  Q,E: 8 steps  F: time");
    screen_at(0, 24);
    put_text("1,2,3: view 0,20,40  RETURN: end");

    view_clear();
    game.view_x = 0;
    for (uint8_t y = 1; y <= MAP_ROWS; y++) {
        for (uint8_t column = 0; column < NUMCOLS; column++) {
            draw_tile(column, y, view_pattern(column, y));
        }
    }
    draw_tile(x, VIEW_TEST_ROW, PLAYER);

    while (code != KEY_RETURN) {
        screen_at(0, 0);
        put_text("view_x:");
        put_uint(game.view_x, 3);
        put_text("  x:");
        put_uint(x, 3);
        put_text("  50 redraws:");
        put_uint(frames, 4);
        if (frames) {
            put_text(frames <= VIEW_MEASURE_LIMIT ? " OK  " : " FAIL");
        }

        do {
            code = key_get();
        } while (!code);

        switch (code) {
            case 'a': x = view_step(x, -1); break;
            case 'd': x = view_step(x, 1); break;
            case 'q': x = view_step(x, -8); break;
            case 'e': x = view_step(x, 8); break;
            case 'f': frames = view_measure(); break;
            case '1': center_view(20); break;
            case '2': center_view(40); break;
            case '3': center_view(60); break;
        }
    }
}
