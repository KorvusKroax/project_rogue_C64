#include "dev.h"

#include <stddef.h>

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"
#include "../platform/charset.h"
#include "../input.h"
#include "../map_signs.h"
#include "../message.h"
#include "../state.h"
#include "../view.h"

// a 95 jel három sorban
static void put_all_glyphs(uint8_t first_row, uint8_t offset)
{
    for (uint8_t ch = GLYPH_FIRST; ch <= GLYPH_LAST; ch++) {
        uint8_t i = ch - GLYPH_FIRST;
        put_screen_character(i % SCREEN_WIDTH, first_row + i / SCREEN_WIDTH, ch + offset);
    }
}

static void dev_charset_screen(void)
{
    screen_clear();

    set_screen_cursor(2, 0);
    put_text("Normal:");
    put_all_glyphs(1, 0);

    set_screen_cursor(2, 5);
    put_text("Inverse:");
    put_all_glyphs(6, 128);

    static const uint8_t map_signs[] = {
        PASSAGE, DOOR, FLOOR, PLAYER, TRAP, STAIRS, GOLD, POTION, SCROLL, WALL_HORIZONTAL, WALL_VERTICAL,
        FOOD, WEAPON, ARMOR, AMULET, RING, STICK, MAGIC, SOLID_ROCK, 'A', 'Z',
    };
    for (uint8_t i = 0; i < sizeof(map_signs); i++) {
        put_screen_character(i, 10, map_signs[i]);
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
        set_screen_cursor(0, 12 + i);
        put_text(map_sample[i]);
    }

    // a sor vége után nem szabad semminek átcsúsznia a következő sorba
    set_screen_cursor(30, 18);
    put_text("Cut at the edge: X");

    // clear_to_row_end teszt
    for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
        put_screen_character(x, 20, 'x');
        put_screen_character(x, 21, 'y');
    }
    set_screen_cursor(20, 20);
    clear_to_row_end();

    // a négy sarok: a KERNAL a jobb alsónál görgetne
    put_screen_character(0, 0, '1');
    put_screen_character(SCREEN_WIDTH - 1, 0, '2');
    put_screen_character(0, SCREEN_HEIGHT - 1, '3');
    put_screen_character(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, '4');
}

static void dev_keyboard_screen(void)
{
    uint8_t last_code = 0, presses = 0;

    screen_clear();
    set_screen_cursor(2, 0);
    put_text("Keyboard matrix");
    set_screen_cursor(2, 1);
    put_text("0..7 column, row bits down");
    set_screen_cursor(2, 13);
    put_text("Modifiers:");
    set_screen_cursor(2, 15);
    put_text("Last press:");
    set_screen_cursor(2, 16);
    put_text("Presses:");
    set_screen_cursor(2, 24);
    put_text("ESC to leave, SHIFT: stop reading");

    while (last_code != KEY_ESCAPE) {
        wait_for_next_frame();

        // a SHIFT nyomva tartása alatt a puffer megtelhet
        if (!(key_modifiers & KEY_MODIFIER_SHIFT)) {
            uint8_t code = read_key();
            if (code) {
                last_code = code;
                presses++;
            }
        }

        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
                put_screen_character(2 + column, 3 + row, (key_matrix[column] >> row) & 1 ? '#' : '.');
            }
        }

        set_screen_cursor(13, 13);
        put_text(key_modifiers & KEY_MODIFIER_SHIFT ? "SHIFT " : "      ");
        put_text(key_modifiers & KEY_MODIFIER_CTRL ? "CTRL" : "    ");

        set_screen_cursor(14, 15);
        put_uint(last_code, 3);
        set_screen_cursor(14, 16);
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
    if (target > MAP_COLUMNS - 1) target = MAP_COLUMNS - 1;

    draw_tile(x, VIEW_TEST_ROW, view_pattern(x, VIEW_TEST_ROW));
    draw_tile(target, VIEW_TEST_ROW, PLAYER);
    follow_view_window(target);

    return target;
}

#define VIEW_MEASURE_LIMIT  100     // görgetésenként legfeljebb 2 képkocka

// 50 teljes újrarajzolás két távoli nézet között, a képkockák száma
static uint8_t view_measure(void)
{
    uint8_t saved_view_window_x = game.view_window_x;
    uint8_t start = frame_count;

    for (uint8_t i = 0; i < 50; i++) {
        game.view_window_x = i & 1 ? 0 : 40;
        redraw_map();
    }

    uint8_t frames = frame_count - start;
    game.view_window_x = saved_view_window_x;
    redraw_map();
    return frames;
}

// az üzenetsáv 2. sorát utánozza
static void view_toggle_top_row(bool hidden)
{
    set_map_top_row_visibility(!hidden);

    if (hidden) {
        set_screen_cursor(0, MESSAGE_ROW + 1);
        put_text("second message row");
        clear_to_row_end();
    }
}

static void view_draw_pattern(void)
{
    clear_seen_map();
    game.view_window_x = 0;
    for (uint8_t y = 1; y <= MAP_ROWS; y++) {
        for (uint8_t column = 0; column < MAP_COLUMNS; column++) {
            draw_tile(column, y, view_pattern(column, y));
        }
    }
}

static void dev_view_screen(void)
{
    uint8_t code = 0, x = 20, frames = 0;
    bool top_row_hidden = false;

    screen_clear();
    set_screen_cursor(0, 23);
    put_text("A,D: step  Q,E: 8 steps  F: time");
    set_screen_cursor(0, 24);
    put_text("1,2,3: view 0,20,40  H: row 1  ESC: end");

    view_draw_pattern();
    draw_tile(x, VIEW_TEST_ROW, PLAYER);

    while (code != KEY_ESCAPE) {
        set_screen_cursor(0, 0);
        put_text("view_x:");
        put_uint(game.view_window_x, 3);
        put_text("  x:");
        put_uint(x, 3);
        put_text("  50 redraws:");
        put_uint(frames, 4);
        if (frames) {
            put_text(frames <= VIEW_MEASURE_LIMIT ? " OK  " : " FAIL");
        }

        code = wait_for_key();

        switch (code) {
            case 'a': x = view_step(x, -1); break;
            case 'd': x = view_step(x, 1); break;
            case 'q': x = view_step(x, -8); break;
            case 'e': x = view_step(x, 8); break;
            case 'f': frames = view_measure(); break;
            case 'h':
                top_row_hidden = !top_row_hidden;
                view_toggle_top_row(top_row_hidden);
                break;
            case '1': center_view_window(20); break;
            case '2': center_view_window(40); break;
            case '3': center_view_window(60); break;
        }
    }

    set_map_top_row_visibility(true);
}

#define TEST_FIELD_LENGTH  20

// a beírt szöveg üzenetként jelenik meg; ESC után nincs mit kiírni
static void ask_test_field(const char *prompt)
{
    char answer[TEST_FIELD_LENGTH + 1];

    if (ask_text(prompt, answer, TEST_FIELD_LENGTH)) {
        message(answer);
    }
}

static void dev_message_screen(void)
{
    uint8_t code = 0;

    screen_clear();
    set_screen_cursor(0, 23);
    put_text("1..6: messages  7,8,9: prompts");
    set_screen_cursor(0, 24);
    put_text("Q,E: view 0,40  ESC: end  other: clear");

    view_draw_pattern();

    while (code != KEY_ESCAPE) {
        code = wait_for_key_and_clear_message();

        switch (code) {
            case '1':
                message("you miss the bat");
                message_format("\1 bites you for \3 points", "the bat", NULL, 3);
                break;
            case '2':
                message("you miss the bat");
                message("you are now wearing a very long piece of armor and it fits you");
                break;
            case '3': message("this message fills the row exactly, ok."); break;
            case '4': message("this very long message does not fit in the two rows of the bar, so it goes on to the next page after the last word"); break;
            case '5':
                message("you feel a wrenching sensation");
                message("you are now wearing a very long piece of armor and it fits you");
                break;
            case '6': message("a) a mace"); break;
            case '7': ask_key("which object do you want to quaff? (* for list): "); break;
            case '8': ask_test_field("call it: "); break;   // a mező elfér a 0. sorban
            case '9': ask_test_field("this question is long enough to go on to the second row, name it: "); break; // a mező az 1. sor végéig tart
            case 'q': center_view_window(0); break;
            case 'e': center_view_window(MAP_COLUMNS - 1); break;
        }
    }

    message_clear();
}

void dev_run_screens(void)
{
    screen_clear();

    set_screen_cursor(0, 0);
    put_text("Seed: ");
    put_uint(game.seed, 0);
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();

    put_text("Tests:");
    move_screen_cursor_to_next_row();
    if (!dev_run_tests()) {
        put_text("Selftest failed");
        move_screen_cursor_to_next_row();
    }
    move_screen_cursor_to_next_row();
    put_text("Press a key");
    wait_for_key();

    dev_charset_screen();
    set_screen_cursor(2, 24);
    put_text("Press a key");
    wait_for_key();

    dev_keyboard_screen();

    dev_view_screen();

    dev_message_screen();
}
