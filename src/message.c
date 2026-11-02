#include "message.h"

#include <stdbool.h>

#include "platform/screen.h"
#include "view.h"

static bool second_row_used;

uint8_t message_line_length(const char *text)
{
    uint8_t length = 0;
    uint8_t last_space = 0;

    while (length < SCREEN_W) {
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

    return last_space ? last_space : SCREEN_W;
}

static void show_line(uint8_t row, const char *text, uint8_t length)
{
    for (uint8_t x = 0; x < length; x++) {
        screen_put(x, row, text[x]);
    }

    screen_at(length, row);
    clear_to_line_end();
}

void message_show(const char *text)
{
    message_clear();

    uint8_t length = message_line_length(text);
    show_line(MESSAGE_ROW, text, length);

    text += length;
    if (*text == ' ') {
        text++;
    }

    if (*text) {
        second_row_used = true;
        view_hide_top_row(true);
        show_line(MESSAGE_ROW + 1, text, message_line_length(text));
    }
}

void message_clear(void)
{
    screen_at(0, MESSAGE_ROW);
    clear_to_line_end();

    if (second_row_used) {
        second_row_used = false;
        view_hide_top_row(false);
    }
}
