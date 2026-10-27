#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#define STATE_VERSION 1

// a játékállapot-blokk: a mentés és a betöltés egyben írja és olvassa
// (kezdőcím: &game, méret: sizeof(game))
// csak ide kerülhet perzisztens adat; mutató nem, csak index
struct GameState {
    uint32_t seed;
    uint8_t  level;
};

extern struct GameState game;

#endif
