#include "tests.h"

#include <stddef.h>

#include "../platform/platform.h"
#include "../platform/screen.h"
#include "../platform/key.h"
#include "../platform/charset.h"
#include "../input.h"
#include "../map.h"
#include "../message.h"
#include "../num.h"
#include "../state.h"
#include "../status.h"
#include "../view.h"

// minden tesztképernyőről az ESC lép ki
static void wait_for_escape(void)
{
    while (wait_for_key() != KEY_ESCAPE) {
    }
}

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
    put_all_glyphs(6, INVERSE_OFFSET);

    static const uint8_t map_signs[] = {
        CHAR_PASSAGE, CHAR_DOOR, CHAR_FLOOR, CHAR_PLAYER, CHAR_TRAP, CHAR_STAIRS, CHAR_GOLD, CHAR_POTION, CHAR_SCROLL, CHAR_WALL_HORIZONTAL, CHAR_WALL_VERTICAL,
        CHAR_FOOD, CHAR_WEAPON, CHAR_ARMOR, CHAR_AMULET, CHAR_RING, CHAR_STICK, CHAR_MAGIC, CHAR_SOLID_ROCK, 'A', 'Z',
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
    for (uint8_t i = 0; i < sizeof(map_sample) / sizeof(map_sample[0]); i++) {
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

    set_screen_cursor(2, SCREEN_HEIGHT - 1);
    put_text("ESC: end");
    wait_for_escape();
}

static void dev_keyboard_screen(void)
{
    uint8_t last_code = 0, presses = 0;
    uint8_t frames = 0, gap = 0;    // a két utolsó leütés között eltelt képkockák
    bool repeat = false;

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
    set_screen_cursor(2, 17);
    put_text("Frames between:");
    set_screen_cursor(2, 18);
    put_text("Repeat:");
    set_screen_cursor(2, 23);
    put_text("R: repeat on/off");
    set_screen_cursor(2, 24);
    put_text("ESC to leave, SHIFT: stop reading");

    while (last_code != KEY_ESCAPE) {
        wait_for_next_frame();
        if (frames < 255) {
            frames++;
        }

        // a SHIFT nyomva tartása alatt a puffer megtelhet
        if (!(key_modifiers & KEY_MODIFIER_SHIFT)) {
            uint8_t code = read_key();
            if (code) {
                last_code = code;
                presses++;
                gap = frames;
                frames = 0;

                if (code == 'r') {
                    repeat = !repeat;
                    set_key_repeat(repeat);
                }
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
        put_int(last_code, 3);
        set_screen_cursor(14, 16);
        put_int(presses, 3);
        set_screen_cursor(18, 17);
        put_int(gap, 3);
        set_screen_cursor(10, 18);
        put_text(repeat ? "on " : "off");
    }

    set_key_repeat(false);
}

// a térkép első két sora az oszlop száma, a többi betűje tízesenként vált
// __noinline: a view_draw_pattern ciklusába inline-olva az Oscar64 -O2 rossz kódot ad (README, Oscar64-hibák 7.)
static __noinline uint8_t view_pattern(uint8_t x, uint8_t y)
{
    if (y == MAP_TOP_ROW) return '0' + x / 10;
    if (y == MAP_TOP_ROW + 1) return '0' + x % 10;
    return x % 10 == 0 ? '|' : 'a' + x / 10;
}

#define VIEW_TEST_ROW  (MAP_TOP_ROW + MAP_ROWS / 2)     // a térkép középső sora

// a jel a mintát visszaállítva lép, a széleken megáll
static uint8_t view_step(uint8_t x, int8_t step)
{
    int8_t target = x + step;
    if (target < 0) target = 0;
    if (target > MAP_COLUMNS - 1) target = MAP_COLUMNS - 1;

    draw_tile(x, VIEW_TEST_ROW, view_pattern(x, VIEW_TEST_ROW));
    draw_tile(target, VIEW_TEST_ROW, CHAR_PLAYER);
    follow_view_window(target);
    show_map_changes();

    return target;
}

#define VIEW_MEASURE_REDRAW_COUNT       50
#define VIEW_MEASURE_FRAMES_PER_REDRAW  2
#define VIEW_MEASURE_LIMIT              (VIEW_MEASURE_REDRAW_COUNT * VIEW_MEASURE_FRAMES_PER_REDRAW)

// a képkockaszámláló 8 bites
#if VIEW_MEASURE_LIMIT > 255
    #error "a VIEW_MEASURE_LIMIT nem fér el 8 biten"
#endif

// VIEW_MEASURE_REDRAW_COUNT teljes újrarajzolás két távoli nézet között, a képkockák száma
static uint8_t view_measure(void)
{
    uint8_t saved_view_window_x = game.view_window_x;
    uint8_t start = frame_count;

    for (uint8_t i = 0; i < VIEW_MEASURE_REDRAW_COUNT; i++) {
        game.view_window_x = i & 1 ? 0 : VIEW_WINDOW_X_MAX;
        redraw_map();
    }

    uint8_t frames = frame_count - start;
    game.view_window_x = saved_view_window_x;
    redraw_map();
    return frames;
}

// a minta és a jel a SCREEN rétegbe; a kirajzolás a hívó dolga
static void view_draw_pattern(uint8_t player_x)
{
    clear_seen_map();
    game.view_window_x = 0;
    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t column = 0; column < MAP_COLUMNS; column++) {
            draw_tile(column, y, view_pattern(column, y));
        }
    }
    draw_tile(player_x, VIEW_TEST_ROW, CHAR_PLAYER);
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

// a nézőablak helye, a jel helye és az utolsó mérés a státuszsor első sorában
static void put_view_state(uint8_t x, uint8_t frames)
{
    set_screen_cursor(0, STATUS_ROW);
    put_text("view_x:");
    put_int(game.view_window_x, 3);
    put_text("  x:");
    put_int(x, 3);
    put_text("  redraws:");
    put_int(frames, 4);
    if (frames) {
        put_text(frames <= VIEW_MEASURE_LIMIT ? " OK  " : " FAIL");
    }
    clear_to_row_end();
}

// üzenetsáv és nézőablak: az üzenetek a térképminta fölött, a jel lépésére görgető nézőablakkal
static void dev_message_and_view_screen(void)
{
    uint8_t code = 0, x = VIEW_WINDOW_WIDTH / 2, frames = 0;

    screen_clear();
    set_screen_cursor(0, STATUS_ROW + 1);
    put_text("1-9:msg A,D,Q,E:step Z,X,C:view F:time");

    view_draw_pattern(x);
    redraw_map();

    while (code != KEY_ESCAPE) {
        put_view_state(x, frames);
        code = wait_for_key_and_clear_message();

        switch (code) {
            case '1':
                message("you miss the bat");
                message_format("\1 bites you", "the bat", NULL, 0);
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
            case 'a': x = view_step(x, -1); break;
            case 'd': x = view_step(x, 1); break;
            case 'q': x = view_step(x, -8); break;
            case 'e': x = view_step(x, 8); break;
            case 'z': center_view_window(VIEW_WINDOW_WIDTH / 2); break;
            case 'x': center_view_window(MAP_COLUMNS / 2); break;
            case 'c': center_view_window(MAP_COLUMNS - VIEW_WINDOW_WIDTH / 2); break;
            case 'f': frames = view_measure(); break;
        }
    }

    message_clear();
}

static void set_status_test_values(uint8_t dungeon_level, uint32_t gold, int16_t hit_points, int16_t max_hit_points, uint8_t strength, uint8_t max_strength, uint8_t experience_level, uint32_t experience)
{
    game.dungeon_level = dungeon_level;
    game.gold = gold;
    game.player.hit_points = hit_points;
    game.player.max_hit_points = max_hit_points;
    game.player.strength = strength;
    game.max_strength = max_strength;
    game.player.experience_level = experience_level;
    game.player.experience = experience;
}

#define STATUS_MEASURE_DRAW_COUNT  50

// STATUS_MEASURE_DRAW_COUNT kirajzolás változatlan értékekkel, a képkockák száma
static uint8_t status_measure(void)
{
    uint8_t start = frame_count;

    for (uint8_t i = 0; i < STATUS_MEASURE_DRAW_COUNT; i++) {
        draw_status();
    }

    return frame_count - start;
}

static void dev_status_screen(void)
{
    uint8_t code = 0, frames = 0;

    screen_clear();
    set_screen_cursor(0, 0);
    put_text("1: start  2: typical  3: widest real");
    set_screen_cursor(0, 1);
    put_text("4: extreme  H: hunger  F: time  ESC: end");

    // oszlopvonalzó a státuszsor fölött
    for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
        put_screen_character(x, STATUS_ROW - 2, '0' + (x + 1) / 10 % 10);
        put_screen_character(x, STATUS_ROW - 1, '0' + (x + 1) % 10);
    }

    set_status_test_values(1, 0, 12, 12, 16, 16, 1, 0);
    game.hunger_state = 0;

    while (code != KEY_ESCAPE) {
        draw_status();

        set_screen_cursor(0, 3);
        put_int(STATUS_MEASURE_DRAW_COUNT, 0);
        put_text(" draws: ");
        put_int(frames, 3);
        put_text(" frames");

        code = wait_for_key();

        switch (code) {
            case '1': set_status_test_values(1, 0, 12, 12, 16, 16, 1, 0); break;
            case '2': set_status_test_values(3, 25, 14, 20, 16, 16, 2, 38); break;
            case '3': set_status_test_values(255, 999999, 999, 999, 31, 31, 21, 16777215); break;
            case '4': set_status_test_values(255, 16777215, -12, 999, 3, 31, 21, 16777215); break;  // 8 jegyű arany: a sor vége levágódik
            case 'h': game.hunger_state = (game.hunger_state + 1) & 3; break;
            case 'f': frames = status_measure(); break;
        }
    }
}

static void draw_menu(void)
{
    screen_clear();

    set_screen_cursor(0, 0);
    put_text("Test screens:");
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();
    put_text("C: charset  K: keyboard  S: status");
    move_screen_cursor_to_next_row();
    put_text("M: message bar and view window");
}

void run_test_screens(void)
{
    while (true) {
        draw_menu();

        switch (wait_for_key()) {
            case 'c': dev_charset_screen(); break;
            case 'k': dev_keyboard_screen(); break;
            case 'm': dev_message_and_view_screen(); break;
            case 's': dev_status_screen(); break;
        }
    }
}
