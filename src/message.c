#include "message.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "platform/key.h"
#include "platform/screen.h"
#include "num.h"
#include "status.h"
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
static uint8_t message_text_length;
static char message_bar_text[SCREEN_WIDTH * MESSAGE_ROW_COUNT + 1];
static uint16_t message_bar_length;
static uint8_t message_bar_end_column;
static uint8_t message_bar_rows_in_use = 1;     // ennyi sort foglal a sáv; a többi a térképé

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

const char *prepare_message_text(const char *text, const char *first_text, const char *second_text, int16_t number, bool add_end_character)
{
    fill_message_text(text, first_text, second_text, number);

    uint8_t length = message_text_length;

    // a tárgylista-sor (pl. "a) a mace") változatlan marad
    if (length && message_text[1] != ')') {
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

// az utolsó sorban a MORE_TEXT helye is megmarad
uint16_t calculate_message_chars_fitting_bar(const char *text)
{
    const char *row_text = text;
    for (uint8_t row = 0; row < MESSAGE_ROW_COUNT - 1; row++) {
        row_text = text_after_break(row_text + calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH));
    }

    return (row_text - text) + calculate_message_chars_fitting_row(row_text, SCREEN_WIDTH - MORE_TEXT_LENGTH);
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

static void show_more_and_wait(void)
{
    // ha az utolsó használt sorban nincs helye, a következő sor elejére kerül; a sáv utolsó sorában mindig van helye
    if (message_bar_end_column > SCREEN_WIDTH - MORE_TEXT_LENGTH) {
        set_screen_cursor(0, MESSAGE_ROW + message_bar_rows_in_use);
        set_message_bar_rows_in_use(message_bar_rows_in_use + 1);
    }

    put_text(MORE_TEXT);
    clear_to_row_end();

    while (wait_for_key() != ' ') {}

    message_clear();
}

// az eddigi szöveg után fűzi, ha a MORE_TEXT is elfér vele
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

static void add_to_message_bar(const char *text)
{
    if (message_bar_length) {
        if (append_to_message_bar(text)) {
            return;
        }
        show_more_and_wait();
    }

    while (true) {
        message_bar_length = calculate_message_chars_fitting_bar(text);
        memcpy(message_bar_text, text, message_bar_length);
        message_bar_text[message_bar_length] = 0;
        show_message_bar();

        text = text_after_break(text + message_bar_length);
        if (!*text) {
            return;
        }
        show_more_and_wait();
    }
}

void message(const char *text)
{
    message_format(text, NULL, NULL, 0);
}

void message_format(const char *text, const char *first_text, const char *second_text, int16_t number)
{
    add_to_message_bar(prepare_message_text(text, first_text, second_text, number, true));
}

void message_prompt(const char *text)
{
    add_to_message_bar(prepare_message_text(text, NULL, NULL, 0, false));
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
