#include "message.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "platform/key.h"
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
#define MESSAGE_END_CHARACTER   '.'

#define PLACEHOLDER_FIRST_TEXT  '\1'
#define PLACEHOLDER_SECOND_TEXT '\2'
#define PLACEHOLDER_NUMBER      '\3'

static char message_text[MESSAGE_TEXT_SIZE];
static char last_message_text[MESSAGE_TEXT_SIZE];   // kiírt alakjában, a CTRL-P-hez
static uint8_t message_text_length;
static char message_bar_text[SCREEN_WIDTH * MESSAGE_ROW_COUNT + 1];
static uint16_t message_bar_length;
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
    if (message_text_length < MESSAGE_TEXT_SIZE - 2) {  // a záró pontnak és nullának hely kell
        message_text[message_text_length++] = character;
    }
}

static void add_text_to_message_text(const char *text)
{
    while (*text) {
        add_character_to_message_text(*text++);
    }
}

static void fill_message_text(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    char number_text[FORMAT_BUFFER_SIZE];

    message_text_length = 0;

    while (*text) {
        switch (*text) {
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
                add_character_to_message_text(*text);
                break;
        }
        text++;
    }

    message_text[message_text_length] = 0;
}

// a tárgylista-sor (pl. "a) a mace"): nem kap nagybetűt és záró pontot, és nem fűződik össze más üzenettel
static bool is_list_row(const char *text)
{
    return text[0] && text[1] == ')';
}

const char *prepare_message_text(const char *text, const char *first_text, const char *second_text, int16_t number, bool add_end_character)
{
    fill_message_text(text, first_text, second_text, number);

    uint8_t length = message_text_length;

    if (length && !is_list_row(message_text)) {
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

// az eddigi szöveg után fűzi, ha a TEXT_MORE is elfér vele
static bool append_to_message_bar(const char *text)
{
    uint16_t joined_length = message_bar_length + 1 + strlen(text);
    if (joined_length >= sizeof(message_bar_text)) {
        return false;
    }

    message_bar_text[message_bar_length] = ' ';
    strcpy(message_bar_text + message_bar_length + 1, text);

    if (calculate_message_chars_fitting_bar(message_bar_text) < joined_length) {
        message_bar_text[message_bar_length] = 0;
        return false;
    }

    message_bar_length = joined_length;
    show_message_bar();
    return true;
}

// a szöveg a sávba, az eddigi után, ha elfér, különben --More-- után; a szöveg előtti --More---nál is_escape_allowed mellett
// ESC-re a szöveg elmarad, és a sáv üres; hamis, ha így elmaradt
static bool add_to_message_bar(const char *text, bool is_escape_allowed)
{
    if (message_bar_length) {
        // a tárgylista-sor után nem fűződik semmi, és ő sem fűződik az eddigi szöveghez; így mindig a sáv elején áll
        if (!is_list_row(message_bar_text) && !is_list_row(text) && append_to_message_bar(text)) {
            return true;
        }
        if (!show_more_and_wait(is_escape_allowed)) return false;
    }

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
    add_to_message_bar(prepared_text, false);
}

void message_format_without_recall(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    add_to_message_bar(prepare_message_text(text, first_text, second_text, number, true), false);
}

bool message_unless_escaped(const char *text)
{
    const char *prepared_text = prepare_message_text(text, NULL, NULL, 0, true);
    if (!add_to_message_bar(prepared_text, true)) return false;

    strcpy(last_message_text, prepared_text);
    return true;
}

void repeat_last_message(void)
{
    message(last_message_text);
}

void message_prompt(const char *text)
{
    message_prompt_format(text, NULL);
}

void message_prompt_format(const char *text, const char *first_text)
{
    add_to_message_bar(prepare_message_text(text, first_text, NULL, 0, false), false);
    message_bar_length = 0;     // a kérdés mögé nem fűződik semmi, a következő üzenet lecseréli
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
