#include "dev.h"

#include "../platform/key.h"
#include "../input.h"
#include "../level.h"
#include "../message.h"
#include "../state.h"

#define DEPTH_DIGITS_MAX  3     // 1..255

// ESC-re vagy hibás számra nem történik semmi
static void jump_to_depth(void)
{
    char answer[DEPTH_DIGITS_MAX + 1];
    uint32_t depth;

    bool answered = ask_text("depth (1-255): ", answer, DEPTH_DIGITS_MAX);
    message_clear();

    if (answered && convert_text_to_number(answer, 255, &depth) && depth >= 1) {
        game.dungeon_level = depth;
        new_level();
    }
}

bool dev_execute_command(uint8_t code)
{
    switch (code) {
        case KEY_F1:
            new_level();
            break;
        case KEY_F3:
            jump_to_depth();
            break;
    }

    return false;
}
