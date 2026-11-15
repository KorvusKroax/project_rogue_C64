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

#define REPEAT_COUNT_MAX  255

// a számprefixes ismétlés: hányszor jön még a parancs (futás közben nem fogy)
static uint8_t repeat_count;
static uint8_t repeat_code;

// az `a` ezt ismétli; 0: még nem volt parancs
static uint8_t last_code;

void stop_running_and_repeating(void)
{
    stop_running();
    repeat_count = 0;
}

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
    repeat_count = 0;

    char name[KEY_NAME_SIZE];
    write_key_name(name, code);
    message_format_without_recall(text_illegal_command, name, NULL, 0);
}

// a parancsok, amelyeknél a számprefix megmarad
static bool is_repeatable_command(uint8_t code)
{
    switch (code) {
        case 'h': case 'j': case 'k': case 'l': case 'y': case 'u': case 'b': case 'n':
        case 'H': case 'J': case 'K': case 'L': case 'Y': case 'U': case 'B': case 'N':
        case KEY_CTRL('h'): case KEY_CTRL('j'): case KEY_CTRL('k'): case KEY_CTRL('l'):
        case KEY_CTRL('y'): case KEY_CTRL('u'): case KEY_CTRL('b'): case KEY_CTRL('n'):
        case '.': case 'a': case 's':
            return true;
        default:
            return false;
    }
}

static bool is_digit_key(uint8_t code)
{
    return code >= '0' && code <= '9';
}

// a következő parancs a billentyűzetről, az esetleges számprefixszel együtt
static uint8_t read_command(void)
{
    set_key_repeat(true);
    uint8_t code = wait_for_key();
    message_clear();

    if (is_digit_key(code)) {
        uint8_t count = 0;
        while (is_digit_key(code)) {
            uint8_t digit = code - '0';
            if (count > (REPEAT_COUNT_MAX - digit) / 10) {
                count = REPEAT_COUNT_MAX;
            } else {
                count = count * 10 + digit;
            }
            code = wait_for_key();
        }
        // az első végrehajtás is a számból fogy
        if (count > 0 && is_repeatable_command(code)) {
            repeat_count = count - 1;
            repeat_code = code;
        }
    }
    set_key_repeat(false);

    if (code == 'a' || code == KEY_ESCAPE) return code;
    #ifdef DEVMODE
        // a fejlesztői parancsokat az `a` nem ismétli
        if (is_function_key(code)) return code;
    #endif
    last_code = code;
    return code;
}

// a parancs végrehajtása; igaz, ha időt vitt
static bool execute_command(uint8_t code)
{
    if (code == 'a') {
        if (last_code == 0) {
            message(text_no_command_yet);
            return false;
        }
        code = last_code;
    }

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
        case 's':
            search_around();
            return true;
        case '>':
            go_down_stairs();
            return false;
        case '<':
            go_up_stairs();
            return false;
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
        // futás és ismétlés közben jump_to_run_end mellett a képernyő csak a végén frissül
        bool is_repeating = is_running() || repeat_count > 0;
        if (!(is_repeating && (options.flags & OPTION_JUMP_TO_RUN_END))) {
            show_map_changes();
            draw_status();
        }

        // futás és ismétlés közben nincs billentyűolvasás: az előre leütött billentyűk a pufferben várnak
        bool took_time;
        if (is_running()) {
            took_time = take_run_step();
        } else if (repeat_count > 0) {
            repeat_count--;
            took_time = execute_command(repeat_code);
        } else {
            took_time = execute_command(read_command());
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
