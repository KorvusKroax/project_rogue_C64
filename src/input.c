#include "input.h"

#include "platform/key.h"
#include "platform/screen.h"
#include "message.h"

#define INPUT_CURSOR (' ' + 128)    // inverz szóköz

static uint8_t field_x;
static uint8_t field_y;

static void start_field(void)
{
    field_x = get_screen_cursor_x();
    field_y = get_screen_cursor_y();
}

// a mező a sor végén a következő sor elején folytatódik
static void put_field_character(uint8_t index, uint8_t character)
{
    uint8_t x = field_x + index;
    put_screen_character(x % SCREEN_WIDTH, field_y + x / SCREEN_WIDTH, character);
}

static bool is_printable(uint8_t code)
{
    return code >= ' ' && code <= '~';
}

bool input_text(char *buffer, uint8_t max_length, uint8_t last_row)
{
    start_field();

    uint8_t capacity = (last_row - field_y + 1) * SCREEN_WIDTH - field_x;
    if (max_length > capacity) {
        max_length = capacity;
    }

    uint8_t length = 0;

    while (true) {
        if (length < max_length) {
            put_field_character(length, INPUT_CURSOR);
        }

        uint8_t code = wait_for_key();

        if (length < max_length) {
            put_field_character(length, ' ');
        }

        if (code == KEY_RETURN) {
            buffer[length] = 0;
            return true;
        }

        if (code == KEY_ESCAPE) {
            buffer[0] = 0;
            return false;
        }

        if (code == KEY_DELETE) {
            if (length) {
                length--;
                put_field_character(length, ' ');
            }
        } else if (is_printable(code) && length < max_length) {
            buffer[length] = code;
            put_field_character(length, code);
            length++;
        }
    }
}

bool ask_text(const char *prompt, char *buffer, uint8_t max_length)
{
    message_prompt(prompt);
    message_reserve_input_space(max_length);

    bool answered = input_text(buffer, max_length, MESSAGE_ROW + 1);

    if (answered) {
        message_finish_prompt();
    } else {
        message_clear();
    }

    return answered;
}

uint8_t ask_key(const char *prompt)
{
    message_prompt(prompt);

    uint8_t code = wait_for_key();

    if (code == KEY_ESCAPE) {
        message_clear();
        return code;
    }

    if (is_printable(code)) {
        message_reserve_input_space(1);
        start_field();
        put_field_character(0, code);
    }

    message_finish_prompt();
    return code;
}
