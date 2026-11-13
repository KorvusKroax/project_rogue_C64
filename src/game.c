#include "game.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "platform/key.h"
#include "message.h"
#include "player.h"
#include "status.h"
#include "texts.h"

#define KEY_NAME_SIZE  3    // legfeljebb két karakter és a lezáró nulla

// a billentyű kiírható alakja: a vezérlőkód ^ + nagybetű (^A, ^[), a törlés ^?, a többi maga a karakter
static void write_key_name(char *name, uint8_t code)
{
    if (code < ' ') {
        name[0] = '^';
        name[1] = code + '@';
        name[2] = 0;
    } else if (code == KEY_DELETE) {
        name[0] = '^';
        name[1] = '?';
        name[2] = 0;
    } else {
        name[0] = code;
        name[1] = 0;
    }
}

static void report_illegal_command(uint8_t code)
{
    char name[KEY_NAME_SIZE];
    write_key_name(name, code);
    message_format_without_recall(text_illegal_command, name, NULL, 0);
}

// a parancs végrehajtása; igaz, ha időt vitt
static bool execute_command(uint8_t code)
{
    switch (code) {
        case '.':
            return true;
        case KEY_ESCAPE:
        case ' ':
            return false;
        case KEY_CTRL('p'):
            repeat_last_message();
            return false;
        default:
            report_illegal_command(code);
            return false;
    }
}

// egy kör: a BEFORE fázis, a játékos akciói, majd az AFTER fázis
static void play_turn(void)
{
    // BEFORE fázis

    // az ingyenes parancs nem fogyaszt akciót
    uint8_t actions = 1;
    while (actions) {
        draw_status();
        set_key_repeat(true);
        uint8_t code = wait_for_key();
        set_key_repeat(false);

        message_clear();
        if (execute_command(code)) {
            actions--;
        }
    }

    // AFTER fázis
}

void run_game(void)
{
    init_player();

    while (true) {
        play_turn();
    }
}
