#include "message.h"

#include <stdbool.h>
#include <string.h>

#include "platform/key.h"
#include "platform/screen.h"
#include "view.h"

#define MORE_TEXT               "--More--"
#define MORE_TEXT_LENGTH        (sizeof(MORE_TEXT) - 1)
#define MESSAGE_TEXT_SIZE       128
#define MESSAGE_END_CHARACTER   '.'

static char message_text[MESSAGE_TEXT_SIZE];
static char message_bar_text[SCREEN_WIDTH * 2 + 1];
static uint8_t message_bar_length;
static uint8_t message_bar_end_column;
static bool second_row_reserved;

const char *prepare_message_text(const char *text)
{
    uint8_t length = 0;

    while (text[length] && length < MESSAGE_TEXT_SIZE - 2) {    // a záró pontnak és nullának hely kell
        message_text[length] = text[length];
        length++;
    }

    // a tárgylista-sor (pl. "a) a mace") változatlan marad
    if (length && text[1] != ')') {
        if (message_text[0] >= 'a' && message_text[0] <= 'z') {
            message_text[0] -= 'a' - 'A';
        }

        char last = message_text[length - 1];
        if (last != '.' && last != '!' && last != '?') {
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

uint8_t calculate_message_chars_fitting_bar(const char *text)
{
    const char *second_row_text = text_after_break(text + calculate_message_chars_fitting_row(text, SCREEN_WIDTH));

    return (second_row_text - text) + calculate_message_chars_fitting_row(second_row_text, SCREEN_WIDTH - MORE_TEXT_LENGTH);
}

static void reserve_second_row(void)
{
    second_row_reserved = true;
    set_map_top_row_visibility(false);
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

static void show_message_bar(void)
{
    uint8_t first_row_length = calculate_message_chars_fitting_row(message_bar_text, SCREEN_WIDTH);
    show_row(MESSAGE_ROW, message_bar_text, first_row_length);

    const char *second_row_text = text_after_break(message_bar_text + first_row_length);
    if (*second_row_text) {
        reserve_second_row();
        show_row(MESSAGE_ROW + 1, second_row_text, strlen(second_row_text));
    }
}

static void show_more_and_wait(void)
{
    if (message_bar_end_column > SCREEN_WIDTH - MORE_TEXT_LENGTH) {
        reserve_second_row();
        set_screen_cursor(0, MESSAGE_ROW + 1);
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

void message(const char *text)
{
    text = prepare_message_text(text);

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

void message_clear(void)
{
    message_bar_length = 0;

    set_screen_cursor(0, MESSAGE_ROW);
    clear_to_row_end();

    if (second_row_reserved) {
        second_row_reserved = false;
        set_map_top_row_visibility(true);
    }
}

uint8_t wait_for_key_and_clear_message(void)
{
    uint8_t code = wait_for_key();
    message_clear();
    return code;
}
