#include "message.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "platform/key.h"
#include "platform/platform.h"
#include "platform/screen.h"
#include "num.h"
#include "status.h"
#include "texts.h"
#include "view.h"

// a sáv legalább egy sor, és nem érhet a státuszsorig
#if MESSAGE_ROW_COUNT < 1 || MESSAGE_ROW + MESSAGE_ROW_COUNT > STATUS_ROW
    #error "az üzenetsáv nem fér el a képernyőn"
#endif

#define MESSAGE_TEXT_SIZE       128
#define MESSAGE_TEXT_LENGTH_MAX (MESSAGE_TEXT_SIZE - 2)     // a kitöltött szöveg után a záró pontnak és a nullának hely kell
#define MESSAGE_END_CHARACTER   '.'

#define PLACEHOLDER_FIRST_TEXT  '\1'
#define PLACEHOLDER_SECOND_TEXT '\2'
#define PLACEHOLDER_NUMBER      '\3'

static char message_text[MESSAGE_TEXT_SIZE];
static char last_message_text[MESSAGE_TEXT_SIZE];   // kiírt alakjában, a CTRL-P-hez
static uint8_t message_text_length;
static bool is_last_message_list_row;           // a CTRL-P listasorként hozza vissza
static char message_bar_text[SCREEN_WIDTH * MESSAGE_ROW_COUNT + 1];
static uint16_t message_bar_length;
static bool is_message_bar_list_row;            // a sáv szövege listasor: semmi nem fűződik hozzá
static uint8_t message_bar_end_column;
static uint8_t message_bar_rows_in_use = 1;     // ennyi sort foglal a sáv; a többi a térképé

#ifdef DEVMODE
    static bool is_more_wait_enabled = true;

    void set_message_more_wait(bool enabled)
    {
        is_more_wait_enabled = enabled;
    }
#endif

static void add_character_to_message_text(char character)
{
    if (message_text_length < MESSAGE_TEXT_LENGTH_MAX) {
        message_text[message_text_length++] = character;
    }
}

// a szöveg bármely címen lehet (copy_text)
static void add_text_to_message_text(const char *text)
{
    char *end = copy_text(message_text + message_text_length, text, MESSAGE_TEXT_LENGTH_MAX - message_text_length);
    message_text_length = end - message_text;
}

// a mintaszöveg és a helyőrzők szövege bármely címen lehet: a minta előbb a pattern-be kerül, a helyőrzők szövege közvetlenül
// a message_text-be
static void fill_message_text(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    char pattern[MESSAGE_TEXT_SIZE];
    char number_text[FORMAT_BUFFER_SIZE];

    copy_text(pattern, text, sizeof(pattern) - 1);
    message_text_length = 0;

    const char *character = pattern;
    while (*character) {
        switch (*character) {
            case PLACEHOLDER_FIRST_TEXT:
                add_text_to_message_text(first_text);
                break;
            case PLACEHOLDER_SECOND_TEXT:
                add_text_to_message_text(second_text);
                break;
            case PLACEHOLDER_NUMBER:
                format_int(number_text, number, 0);
                add_text_to_message_text(number_text);
                break;
            default:
                add_character_to_message_text(*character);
                break;
        }
        character++;
    }

    message_text[message_text_length] = 0;
}

const char *prepare_message_text(const char *text, const char *first_text, const char *second_text, int16_t number, bool add_end_character)
{
    fill_message_text(text, first_text, second_text, number);

    uint8_t length = message_text_length;

    if (length) {
        if (message_text[0] >= 'a' && message_text[0] <= 'z') {
            message_text[0] -= 'a' - 'A';
        }

        char last = message_text[length - 1];
        if (add_end_character && last != '.' && last != '!' && last != '?') {
            message_text[length++] = MESSAGE_END_CHARACTER;
        }
    }

    message_text[length] = 0;
    return message_text;
}

uint8_t calculate_message_chars_fitting_row(const char *text, uint8_t width)
{
    uint8_t length = 0;
    uint8_t last_space = 0;

    while (length < width) {
        if (!text[length]) {
            return length;
        }
        if (text[length] == ' ') {
            last_space = length;
        }
        length++;
    }

    if (!text[length] || text[length] == ' ') {
        return length;
    }

    return last_space ? last_space : width;
}

static const char *text_after_break(const char *text)
{
    return *text == ' ' ? text + 1 : text;  // a törés szóköze nem kerül a következő sor elejére
}

// az utolsó sorban a TEXT_MORE helye is megmarad
uint16_t calculate_message_chars_fitting_bar(const char *text)
{
    const char *row_text = text;
    for (uint8_t row = 0; row < MESSAGE_ROW_COUNT - 1; row++) {
        row_text = text_after_break(row_text + calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH));
    }

    return (row_text - text) + calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH - TEXT_MORE_LENGTH);
}

// a sáv ennyi sort foglal; a felszabaduló sorok törlődnek, és ahol térkép van, oda visszajön
static void set_message_bar_rows_in_use(uint8_t rows)
{
    for (uint8_t row = rows; row < message_bar_rows_in_use; row++) {
        set_screen_cursor(0, MESSAGE_ROW + row);
        clear_to_row_end();
    }

    message_bar_rows_in_use = rows;
    set_map_first_visible_row(MESSAGE_ROW + rows);
}

static void show_row(uint8_t row, const char *text, uint8_t length)
{
    for (uint8_t x = 0; x < length; x++) {
        put_screen_character(x, row, text[x]);
    }

    set_screen_cursor(length, row);
    clear_to_row_end();
    message_bar_end_column = length;
}

// a calculate_message_chars_fitting_bar szerint elfér: az utolsó sorba a maradék kerül
static void show_message_bar(void)
{
    const char *row_text = message_bar_text;
    uint8_t row = 0;

    while (true) {
        uint8_t length = row == MESSAGE_ROW_COUNT - 1 ? strlen(row_text) : calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH);
        show_row(MESSAGE_ROW + row, row_text, length);
        row++;

        row_text = text_after_break(row_text + length);
        if (!*row_text || row == MESSAGE_ROW_COUNT) {
            break;
        }
    }

    set_message_bar_rows_in_use(row);
}

// a szöveg után a sávban maradó hely karakterekben, a show_message_bar kirajzolása szerint: ide kerül tördelés nélkül a beírt szöveg
static uint16_t calculate_message_bar_space_after(const char *text)
{
    const char *row_text = text;
    uint8_t row = 0;

    while (true) {
        uint8_t length = row == MESSAGE_ROW_COUNT - 1 ? strlen(row_text) : calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH);
        row++;

        row_text = text_after_break(row_text + length);
        if (!*row_text || row == MESSAGE_ROW_COUNT) {
            return (MESSAGE_ROW_COUNT - row) * SCREEN_WIDTH + SCREEN_WIDTH - length;
        }
    }
}

// a --More-- után szóközre vár, is_escape_allowed mellett ESC-re is; utána a sáv üres; hamis, ha ESC jött
static bool show_more_and_wait(bool is_escape_allowed)
{
    #ifdef DEVMODE
        if (!is_more_wait_enabled) {
            message_clear();
            return true;
        }
    #endif

    // az utolsó használt sor végére kerül, ha nincs ott helye, a következő sor elejére; a sáv utolsó sorában mindig van helye;
    // a kurzort mindig oda kell állítani, mert az üzenetek között máshová is kerülhetett (pl. két kör között a kirajzoláskor)
    if (message_bar_end_column > SCREEN_WIDTH - TEXT_MORE_LENGTH) {
        set_screen_cursor(0, MESSAGE_ROW + message_bar_rows_in_use);
        set_message_bar_rows_in_use(message_bar_rows_in_use + 1);
    } else {
        set_screen_cursor(message_bar_end_column, MESSAGE_ROW + message_bar_rows_in_use - 1);
    }

    put_text(TEXT_MORE);
    clear_to_row_end();

    // a várakozás alatt a friss értékek látszanak (pl. a szörnyek támadásai után a HP); a kurzort a --More-- után mozgatja
    draw_status();

    uint8_t code = wait_for_key();
    while (code != ' ' && !(is_escape_allowed && code == KEY_ESCAPE)) {
        code = wait_for_key();
    }

    message_clear();
    return code == ' ';
}

// az eddigi szöveg után fűzi, ha a TEXT_MORE, és a kérdés után az input_length hosszú beviteli mező is elfér vele
static bool append_to_message_bar(const char *text, uint8_t input_length)
{
    uint16_t joined_length = message_bar_length + 1 + strlen(text);
    if (joined_length >= sizeof(message_bar_text)) {
        return false;
    }

    message_bar_text[message_bar_length] = ' ';
    strcpy(message_bar_text + message_bar_length + 1, text);

    if (calculate_message_chars_fitting_bar(message_bar_text) < joined_length
        || calculate_message_bar_space_after(message_bar_text) < input_length) {
        message_bar_text[message_bar_length] = 0;
        return false;
    }

    message_bar_length = joined_length;
    show_message_bar();
    return true;
}

// a szöveg a sávba, az eddigi után, ha elfér (kérdésnél a mögötte álló input_length hosszú beviteli mezővel együtt), különben --More--
// után; a szöveg előtti --More---nál is_escape_allowed mellett ESC-re a szöveg elmarad, és a sáv üres; hamis, ha így elmaradt
static bool add_to_message_bar(const char *text, bool is_escape_allowed, uint8_t input_length, bool is_list_row)
{
    if (message_bar_length) {
        // a listasor után nem fűződik semmi, és ő sem fűződik az eddigi szöveghez; így mindig a sáv elején áll
        if (!is_message_bar_list_row && !is_list_row && append_to_message_bar(text, input_length)) {
            return true;
        }
        if (!show_more_and_wait(is_escape_allowed)) return false;
    }

    is_message_bar_list_row = is_list_row;
    while (true) {
        message_bar_length = calculate_message_chars_fitting_bar(text);
        memcpy(message_bar_text, text, message_bar_length);
        message_bar_text[message_bar_length] = 0;
        show_message_bar();

        text = text_after_break(text + message_bar_length);
        if (!*text) {
            return true;
        }
        show_more_and_wait(false);
    }
}

void message(const char *text)
{
    message_format(text, NULL, NULL, 0);
}

void message_format(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    const char *prepared_text = prepare_message_text(text, first_text, second_text, number, true);
    strcpy(last_message_text, prepared_text);
    is_last_message_list_row = false;
    add_to_message_bar(prepared_text, false, 0, false);
}

void message_format_without_recall(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    add_to_message_bar(prepare_message_text(text, first_text, second_text, number, true), false, 0, false);
}

bool message_list_row(const char *text, bool is_escape_allowed)
{
    if (!add_to_message_bar(text, is_escape_allowed, 0, true)) return false;

    strcpy(last_message_text, text);
    is_last_message_list_row = true;
    return true;
}

void repeat_last_message(void)
{
    if (is_last_message_list_row) {
        add_to_message_bar(last_message_text, false, 0, true);
    } else {
        message(last_message_text);
    }
}

void message_prompt(const char *text)
{
    message_prompt_format(text, NULL);
}

// a kérdés a sávba (add_to_message_bar); mögé nem fűződik semmi, a következő üzenet lecseréli
static void show_prompt(const char *text, uint8_t input_length)
{
    add_to_message_bar(text, false, input_length, false);
    message_bar_length = 0;
}

void message_prompt_format(const char *text, const char *first_text)
{
    show_prompt(prepare_message_text(text, first_text, NULL, 0, false), 0);
}

void message_prompt_with_input_space(const char *text, uint8_t input_length)
{
    show_prompt(prepare_message_text(text, NULL, NULL, 0, false), input_length);
    message_reserve_input_space(input_length);
}

void message_reserve_input_space(uint8_t length)
{
    uint8_t x = get_screen_cursor_x();
    uint8_t y = get_screen_cursor_y();

    // a kurzor sorától ennyi sor kell a mezőnek, legfeljebb a sáv végéig
    uint8_t rows = y - MESSAGE_ROW + (x + length + SCREEN_WIDTH - 1) / SCREEN_WIDTH;
    if (rows > MESSAGE_ROW_COUNT) {
        rows = MESSAGE_ROW_COUNT;
    }

    // az újonnan lefoglalt sorok üresek
    for (uint8_t row = message_bar_rows_in_use; row < rows; row++) {
        set_screen_cursor(0, MESSAGE_ROW + row);
        clear_to_row_end();
    }

    if (rows > message_bar_rows_in_use) {
        set_message_bar_rows_in_use(rows);
    }

    set_screen_cursor(x, y);
}

void message_clear(void)
{
    message_bar_length = 0;

    set_screen_cursor(0, MESSAGE_ROW);
    clear_to_row_end();
    set_message_bar_rows_in_use(1);
}

uint8_t wait_for_key_and_clear_message(void)
{
    uint8_t code = wait_for_key();
    message_clear();
    return code;
}
