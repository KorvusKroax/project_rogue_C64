#ifndef STATE_H
#define STATE_H

#include <stdint.h>

// a logikai térkép mérete; térképmező: x = 0..MAP_COLUMNS-1, y = 1..MAP_ROWS
#define MAP_COLUMNS  80
#define MAP_ROWS     22

// a játékállapot-blokk: a mentés és a betöltés egyben írja és olvassa (kezdőcím: &game, méret: sizeof(game))
// csak ide kerülhet perzisztens adat; mutató nem, csak index
struct GameState {
    uint32_t seed;
    uint8_t  level;
    uint8_t  view_window_x;                     // a nézőablak bal szélének logikai oszlopa
    uint8_t  seen_map[MAP_ROWS][MAP_COLUMNS]; // a SCREEN réteg, a logikai y sora a [y - 1]; 7. bit = inverz
};

extern struct GameState game;

#endif
