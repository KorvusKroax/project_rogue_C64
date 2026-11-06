#ifndef STATE_H
#define STATE_H

#include <stdint.h>

// a logikai térkép mérete; térképmező: x = 0..MAP_COLUMNS-1, y = 1..MAP_ROWS
#define MAP_COLUMNS  80
#define MAP_ROWS     22

// a játékos és a szörnyek közös rekordja
struct Creature {
    uint8_t  strength;                          // 3..31
    uint8_t  experience_level;
    int16_t  hit_points;
    int16_t  max_hit_points;
    uint32_t experience;
};

// a szobák száma: szektoronként egy, a szoba indexe a szektor indexe
#define ROOM_COUNT  9

// szobaflagek
#define ROOM_PHANTOM  0x01                      // nem igazi szoba, csak egy folyosó-csomópont
#define ROOM_DARK     0x02                      // sötét szoba
#define ROOM_MAZE     0x04                      // labirintus

struct Room {
    uint8_t x;                                  // a bal felső sarok, fallal együtt; fantom szobánál maga a folyosó-csomópont
    uint8_t y;
    uint8_t width;                              // a méret falakkal együtt; fantom szobánál 0
    uint8_t height;
    uint8_t flags;
};

// a játékállapot-blokk: a mentés és a betöltés egyben írja és olvassa (kezdőcím: &game, méret: sizeof(game))
// csak ide kerülhet perzisztens adat; mutató nem, csak index
struct GameState {
    uint32_t seed;
    uint8_t  dungeon_level;

    uint32_t gold;
    struct Creature player;
    uint8_t  max_strength;                      // a legnagyobb elért erő, gyűrűk nélkül
    uint8_t  hunger_state;                      // 0: rendben, 1: Hungry, 2: Weak, 3: Faint

    struct Room rooms[ROOM_COUNT];

    uint8_t  view_window_x;                     // a nézőablak bal szélének logikai oszlopa
    uint8_t  seen_map[MAP_ROWS][MAP_COLUMNS];   // a SCREEN réteg, a logikai y sora a [y - 1]; 7. bit = inverz
};

extern struct GameState game;

#endif
