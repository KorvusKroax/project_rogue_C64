#include "game.h"

#include <stdint.h>
#include <stdbool.h>

#include "message.h"
#include "status.h"

// a parancs végrehajtása; igaz, ha időt vitt
static bool execute_command(uint8_t code)
{
    (void)code;
    return false;
}

// egy kör: a BEFORE fázis, a játékos akciói, majd az AFTER fázis
static void play_turn(void)
{
    // BEFORE fázis

    // az ingyenes parancs nem fogyaszt akciót
    uint8_t actions = 1;
    while (actions) {
        draw_status();
        uint8_t code = wait_for_key_and_clear_message();
        if (execute_command(code)) {
            actions--;
        }
    }

    // AFTER fázis
}

void run_game(void)
{
    while (true) {
        play_turn();
    }
}
