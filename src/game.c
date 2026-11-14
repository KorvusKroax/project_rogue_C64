#include "game.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "platform/key.h"
#include "explore.h"
#include "level.h"
#include "message.h"
#include "move.h"
#include "view.h"
#include "player.h"
#include "state.h"
#include "status.h"
#include "texts.h"

#ifdef DEVMODE
    #include "dev/dev.h"
#endif

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
        case 'h': return move_player(-1, 0);
        case 'j': return move_player(0, 1);
        case 'k': return move_player(0, -1);
        case 'l': return move_player(1, 0);
        case 'y': return move_player(-1, -1);
        case 'u': return move_player(1, -1);
        case 'b': return move_player(-1, 1);
        case 'n': return move_player(1, 1);
        // a futás indítása nem visz időt
        case 'H': start_running(-1, 0, false); return false;
        case 'J': start_running(0, 1, false); return false;
        case 'K': start_running(0, -1, false); return false;
        case 'L': start_running(1, 0, false); return false;
        case 'Y': start_running(-1, -1, false); return false;
        case 'U': start_running(1, -1, false); return false;
        case 'B': start_running(-1, 1, false); return false;
        case 'N': start_running(1, 1, false); return false;
        case KEY_CTRL('h'): start_running(-1, 0, true); return false;
        case KEY_CTRL('j'): start_running(0, 1, true); return false;
        case KEY_CTRL('k'): start_running(0, -1, true); return false;
        case KEY_CTRL('l'): start_running(1, 0, true); return false;
        case KEY_CTRL('y'): start_running(-1, -1, true); return false;
        case KEY_CTRL('u'): start_running(1, -1, true); return false;
        case KEY_CTRL('b'): start_running(-1, 1, true); return false;
        case KEY_CTRL('n'): start_running(1, 1, true); return false;
        case '.':
            return true;
        case KEY_ESCAPE:
        case ' ':
            return false;
        case KEY_CTRL('p'):
            repeat_last_message();
            return false;
        default:
            #ifdef DEVMODE
                if (is_function_key(code)) {
                    return dev_execute_command(code);
                }
            #endif
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
        look_around();
        show_map_changes();
        draw_status();

        // futás közben nincs billentyűolvasás: az előre leütött billentyűk a pufferben várnak
        bool took_time;
        if (is_running()) {
            took_time = take_run_step();
        } else {
            set_key_repeat(true);
            uint8_t code = wait_for_key();
            set_key_repeat(false);

            message_clear();
            took_time = execute_command(code);
        }
        if (took_time) {
            actions--;
        }
    }

    // AFTER fázis
}

void run_game(void)
{
    init_player();
    game.dungeon_level = 1;
    new_level();

    while (true) {
        play_turn();
    }
}
