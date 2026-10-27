#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#define STATE_VERSION 1

struct GameState {
    uint32_t seed;
    uint8_t  level;
};

extern struct GameState game;

#endif
