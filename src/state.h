#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#define STATE_VERSION 1

// a logikai térkép mérete; térképmező: x = 0..NUMCOLS-1, y = 1..MAP_ROWS
#define NUMCOLS   80
#define NUMLINES  24
#define MAP_ROWS  22

// a játékállapot-blokk: a mentés és a betöltés egyben írja és olvassa (kezdőcím: &game, méret: sizeof(game))
// csak ide kerülhet perzisztens adat; mutató nem, csak index
struct GameState {
    uint32_t seed;
    uint8_t  level;
    uint8_t  view_x;                     // a nézőablak bal szélének logikai oszlopa
    uint8_t  screen[MAP_ROWS][NUMCOLS];  // a SCREEN réteg, a logikai y sora a [y - 1]; 7. bit = inverz
};

extern struct GameState game;

#endif
