#include "dev.h"

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

// a térkép első két sora az oszlop száma, a többi betűje tízesenként vált
static uint8_t view_pattern(uint8_t x, uint8_t y)
{
    if (y == MAP_TOP_ROW) return '0' + x / 10;
    if (y == MAP_TOP_ROW + 1) return '0' + x % 10;
    return x % 10 == 0 ? '|' : 'a' + x / 10;
}

static void view_draw_pattern(void)
{
    clear_seen_map();
    game.view_window_x = 0;
    for (uint8_t y = MAP_TOP_ROW; y < MAP_TOP_ROW + MAP_ROWS; y++) {
        for (uint8_t column = 0; column < MAP_COLUMNS; column++) {
            draw_tile(column, y, view_pattern(column, y));
        }
    }
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
    set_screen_cursor(0, STATUS_ROW);
    put_text("1..6: messages  7,8,9: prompts");
    set_screen_cursor(0, STATUS_ROW + 1);
    put_text("Q,E: view 0,40  ESC: end  other: clear");

    view_draw_pattern();

    while (code != KEY_ESCAPE) {
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
            case 'q': center_view_window(0); break;
            case 'e': center_view_window(MAP_COLUMNS - 1); break;
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

// a szint-önteszt hosszú (normál sebességen percekig), ezért csak a menüből indul
static void dev_level_test_screen(void)
{
    screen_clear();
    set_screen_cursor(0, SCREEN_HEIGHT - 1);
    put_text("ESC: stop");
    set_screen_cursor(0, 0);
    put_text("Level tests:");
    move_screen_cursor_to_next_row();
    if (!dev_run_level_tests()) {
        put_text("Level test failed");
    }
    set_screen_cursor(0, SCREEN_HEIGHT - 1);
    put_text("ESC: end");
    clear_to_row_end();
    wait_for_escape();
}

// a seed előjel nélküli, a put_int előjeles: az utolsó jegy külön, így a többi elfér 31 biten
static void put_seed(uint32_t seed)
{
    if (seed >= 10) {
        put_int(seed / 10, 0);
    }
    put_int(seed % 10, 0);
}

// a seed kézi megadása a sávban; nem szám vagy ESC esetén marad a régi
static void ask_seed(void)
{
    char answer[FORMAT_DIGITS_MAX + 1];

    if (ask_text("seed: ", answer, FORMAT_DIGITS_MAX)) {
        convert_text_to_number(answer, UINT32_MAX, &game.seed);
    }
    message_clear();
}

// a gyors öntesztek (minden alkalommal újra lefutnak, gyorsak), a seed és a menü;
// ami nem fér ki, az a képernyő tetején kigörget
static void draw_tests_and_menu(void)
{
    screen_clear();

    set_screen_cursor(0, 0);
    put_text("Tests:");
    move_screen_cursor_to_next_row();
    if (!dev_run_tests()) {
        put_failed_test_names("Selftest failed: ");
    }
    move_screen_cursor_to_next_row();

    put_text("Seed: ");
    put_seed(game.seed);
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();

    put_text("L: level tests  C: charset  M: message");
    move_screen_cursor_to_next_row();
    put_text("S: status");
    move_screen_cursor_to_next_row();
    put_text("E: enter seed");
    move_screen_cursor_to_next_row();
    move_screen_cursor_to_next_row();
    put_text("RETURN: start the game");
}

void dev_run_screens(void)
{
    uint8_t code = 0;

    enable_function_keys();     // a fejlesztői parancsokhoz

    while (code != KEY_RETURN) {
        draw_tests_and_menu();
        code = wait_for_key();

        switch (code) {
            case 'e': ask_seed(); break;
            case 'l': dev_level_test_screen(); break;
            case 'c': dev_charset_screen(); break;
            case 'm': dev_message_screen(); break;
            case 's': dev_status_screen(); break;
        }
    }
}
