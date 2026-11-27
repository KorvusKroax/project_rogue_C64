#include "tests.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../platform/screen.h"
#include "../message.h"
#include "../pack.h"
#include "../state.h"
#include "../status.h"
#include "../text_list.h"
#include "../texts.h"
#include "../view.h"

#define LIST_TEST_ENTRY_COUNT_MAX  (SCREEN_HEIGHT + 1)     // egy sorosakból ennyi már nem fér az overwrite ablakba
#define LIST_TEST_PREFIX_LENGTH    3                       // "a) "

#if MAX_PACK > LIST_TEST_ENTRY_COUNT_MAX
    #error "a teli csomag listája nem fér a próbatételek közé"
#endif

// a próbatételek: "a) " után egy vagy két szó; a második szó előtt szóköz
#define SHORT_FIRST_WORD_LENGTH     6                      // egy sor, rövidebb a TEXT_PRESS_SPACE-nél
#define WRAPPED_FIRST_WORD_LENGTH   30                     // két sorra törik a második szó előtt
#define WRAPPED_SECOND_WORD_LENGTH  10
#define FULL_ROW_WORD_LENGTH        (SCREEN_WIDTH - LIST_TEST_PREFIX_LENGTH)     // pontosan egy teljes sor

#define WRAPPED_FIRST_ROW_LENGTH  (LIST_TEST_PREFIX_LENGTH + WRAPPED_FIRST_WORD_LENGTH)

#if WRAPPED_FIRST_ROW_LENGTH + 1 + WRAPPED_SECOND_WORD_LENGTH <= SCREEN_WIDTH
    #error "a törő próbatétel egy sorba férne"
#endif

static char short_entry[LIST_TEST_PREFIX_LENGTH + SHORT_FIRST_WORD_LENGTH + 1];
static char wrapped_entry[LIST_TEST_PREFIX_LENGTH + WRAPPED_FIRST_WORD_LENGTH + 1 + WRAPPED_SECOND_WORD_LENGTH + 1];
static char full_row_entry[SCREEN_WIDTH + 1];

static void build_entry(char *out, uint8_t first_word_length, uint8_t second_word_length)
{
    memcpy(out, "a) ", LIST_TEST_PREFIX_LENGTH);
    uint8_t length = LIST_TEST_PREFIX_LENGTH;
    memset(out + length, 'a', first_word_length);
    length += first_word_length;
    if (second_word_length > 0) {
        out[length++] = ' ';
        memset(out + length, 'b', second_word_length);
        length += second_word_length;
    }
    out[length] = 0;
}

static void build_test_entries(void)
{
    build_entry(short_entry, SHORT_FIRST_WORD_LENGTH, 0);
    build_entry(wrapped_entry, WRAPPED_FIRST_WORD_LENGTH, WRAPPED_SECOND_WORD_LENGTH);
    build_entry(full_row_entry, FULL_ROW_WORD_LENGTH, 0);
}

static const char *test_entries[LIST_TEST_ENTRY_COUNT_MAX];
static uint8_t test_entry_count;

static const char *get_test_entry(uint8_t index)
{
    return index < test_entry_count ? test_entries[index] : NULL;
}

// a lista count egyforma tételből
static void set_same_test_entries(const char *entry, uint8_t count)
{
    for (uint8_t index = 0; index < count; index++) {
        test_entries[index] = entry;
    }
    test_entry_count = count;
}

static void set_two_test_entries(const char *first, const char *second)
{
    test_entries[0] = first;
    test_entries[1] = second;
    test_entry_count = 2;
}

static bool is_test_list_size(uint8_t entry_count, uint8_t row_count, uint8_t width)
{
    struct TextListSize size;
    calculate_text_list_size(get_test_entry, &size);
    return size.entry_count == entry_count && size.row_count == row_count && size.width == width;
}

static bool does_test_list_fit_window(void)
{
    struct TextListSize size;
    calculate_text_list_size(get_test_entry, &size);
    return does_text_list_fit_window(&size);
}

int test_text_list_rows(uint16_t i)
{
    build_test_entries();

    // a rövid sor szélessége a TEXT_PRESS_SPACE hossza
    set_same_test_entries(short_entry, 1);
    bool ok = is_test_list_size(1, 1, TEXT_PRESS_SPACE_LENGTH);

    // a hosszú tétel a szóköznél törik, a folytatósor behúzással kezdődik
    set_two_test_entries(short_entry, wrapped_entry);
    ok = ok && is_test_list_size(2, 3, WRAPPED_FIRST_ROW_LENGTH);

    // a pontosan sornyi tétel nem törik
    set_same_test_entries(full_row_entry, 1);
    ok = ok && is_test_list_size(1, 1, SCREEN_WIDTH);

    // a sorok száma a képernyő magasságánál megáll
    set_same_test_entries(wrapped_entry, SCREEN_HEIGHT);
    return ok && is_test_list_size(SCREEN_HEIGHT, SCREEN_HEIGHT, WRAPPED_FIRST_ROW_LENGTH);
}

int test_text_list_window_fit(uint16_t i)
{
    build_test_entries();

    set_two_test_entries(short_entry, wrapped_entry);
    bool ok = does_test_list_fit_window();

    // a teljes sor mellé nem fér az üres oszlop
    set_two_test_entries(short_entry, full_row_entry);
    ok = ok && !does_test_list_fit_window();

    // a TEXT_PRESS_SPACE sorával együtt a képernyő magassága még elfér, eggyel több sor már nem
    set_same_test_entries(short_entry, SCREEN_HEIGHT - 1);
    ok = ok && does_test_list_fit_window();
    set_same_test_entries(short_entry, SCREEN_HEIGHT);
    ok = ok && !does_test_list_fit_window();

    // a teli csomag hosszú nevű tárgyakkal sem
    set_same_test_entries(wrapped_entry, MAX_PACK);
    return ok && !does_test_list_fit_window();
}

#define SCREEN_SIZE  (SCREEN_WIDTH * SCREEN_HEIGHT)

// a tesztprogram kiírása, amíg a teszt a képernyőt használja, és a jelenet a megnyitás előtt
static uint8_t test_program_screen[SCREEN_SIZE];
static uint8_t scene_screen[SCREEN_SIZE];

static void save_screen(uint8_t *copy)
{
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            copy[y * SCREEN_WIDTH + x] = get_screen_character(x, y);
        }
    }
}

static void load_screen(const uint8_t *copy)
{
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            put_screen_character(x, y, copy[y * SCREEN_WIDTH + x]);
        }
    }
}

static bool is_screen_same_as(const uint8_t *copy)
{
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            if (get_screen_character(x, y) != copy[y * SCREEN_WIDTH + x]) return false;
        }
    }
    return true;
}

// két sort foglal, de a --More-- nélkül is elfér
#define WINDOW_TEST_MESSAGE  "a message that takes both rows of the message bar"

int test_window_restore(uint16_t i)
{
    save_screen(test_program_screen);

    // a jelenet: a látott térkép mintázata, a nézőablak a térkép közepén, üres üzenetsáv, státuszsor
    for (uint8_t y = 0; y < MAP_ROWS; y++) {
        for (uint8_t x = 0; x < MAP_COLUMNS; x++) {
            game.seen_map[y][x] = 'a' + (x + y) % 26;
        }
    }
    game.view_window_x = VIEW_WINDOW_X_MAX / 2;
    message_clear();
    redraw_map();
    draw_status();
    save_screen(scene_screen);

    // a két soros üzenet a térkép felső sorát is eltakarja; a megnyitás törli, az ablak az egész képernyőt teleírja
    message_format_without_recall(WINDOW_TEST_MESSAGE, NULL, NULL, 0);
    open_temporary_window();
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            put_screen_character(x, y, '*');
        }
    }
    close_temporary_window();
    bool is_restored = is_screen_same_as(scene_screen);

    clear_seen_map();
    load_screen(test_program_screen);
    return is_restored;
}

// az üzenetsáv első sora a text-tel kezdődik (a képernyő karakterei ASCII kódok)
bool does_message_bar_start_with(const char *text)
{
    for (uint8_t x = 0; text[x]; x++) {
        if (get_screen_character(x, MESSAGE_ROW) != text[x]) return false;
    }
    return true;
}

bool is_message_bar_empty(void)
{
    for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
        if (get_screen_character(x, MESSAGE_ROW) != ' ') return false;
    }
    return true;
}

int test_list_row_not_joined(uint16_t i)
{
    save_screen(test_program_screen);
    // a --More-- nem vár: a sáv törlődik, és a következő szöveg az elején kezdődik
    set_message_more_wait(false);

    // a listasor után a kérdés új sávban
    message_clear();
    message_list_row("a) a mace", false);
    message_prompt("which object?");
    bool ok = does_message_bar_start_with("Which object?");

    // a listasor sem fűződik az előző üzenethez, és változatlan marad: nem kap nagybetűt és pontot
    message_clear();
    message("you hit it");
    message_list_row("a) a mace", false);
    ok = ok && does_message_bar_start_with("a) a mace ");

    // a betű nélküli listasor (a felfedezések listája) sem fűződik a következő listasorhoz, és a CTRL-P is változatlanul hozza vissza
    message_clear();
    message_list_row("a potion of healing(blue)", false);
    message_list_row("a scroll called mine", false);
    ok = ok && does_message_bar_start_with("a scroll called mine ");
    message_clear();
    repeat_last_message();
    ok = ok && does_message_bar_start_with("a scroll called mine ");

    // két másik üzenet összefűződik
    message_clear();
    message("you hit it");
    message("you miss");
    ok = ok && does_message_bar_start_with("You hit it. You miss.");

    message_clear();
    set_message_more_wait(true);
    load_screen(test_program_screen);
    return ok;
}

int test_cleared_last_message(uint16_t i)
{
    save_screen(test_program_screen);
    set_message_more_wait(false);

    message_clear();
    message("you hit it");
    clear_last_message();
    message_clear();
    repeat_last_message();
    bool ok = is_message_bar_empty();

    message_clear();
    message_list_row("a) a mace", false);
    clear_last_message();
    message_clear();
    repeat_last_message();
    ok = ok && is_message_bar_empty();

    message_clear();
    set_message_more_wait(true);
    load_screen(test_program_screen);
    return ok;
}

#define PROMPT_TEST_INPUT_LENGTH  (NAME_LENGTH_MAX + 1)     // a `c` beviteli mezője a kurzor helyével

// a két eset a két 40 karakteres sorra van kiszámolva: a rövid régi névvel a kérdés után 31 hely marad, a hosszúval 16
#if MESSAGE_ROW_COUNT * SCREEN_WIDTH != 80 || PROMPT_TEST_INPUT_LENGTH <= 16 || PROMPT_TEST_INPUT_LENGTH > 31
    #error "a kérdés tesztje 80 karakteres sávra és 17..31 cellás mezőre számol"
#endif

int test_prompt_input_space(uint16_t i)
{
    save_screen(test_program_screen);
    set_message_more_wait(false);

    // a kérdés az előző üzenet után, ha a mezője is elfér mögötte
    message_clear();
    message("was called \"blue\"");
    message_prompt_with_input_space("what do you want to call it? ", PROMPT_TEST_INPUT_LENGTH);
    bool ok = does_message_bar_start_with("Was called \"blue\". What do you want to");

    // ha a kérdés még elférne, de a mezője nem, a kérdés --More-- után a sáv elején kezdődik
    message_clear();
    message("was called \"abcdefghijklmnopqrst\"");
    message_prompt_with_input_space("what do you want to call it? ", PROMPT_TEST_INPUT_LENGTH);
    ok = ok && does_message_bar_start_with("What do you want to call it? ");

    message_clear();
    set_message_more_wait(true);
    load_screen(test_program_screen);
    return ok;
}
