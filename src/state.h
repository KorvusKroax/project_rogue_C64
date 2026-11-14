#ifndef STATE_H
#define STATE_H

#include <stdint.h>

// a logikai térkép mérete; térképmező: x = 0..MAP_COLUMNS-1, y = MAP_TOP_ROW..MAP_TOP_ROW+MAP_ROWS-1
#define MAP_COLUMNS  80                         // default: 80
#define MAP_ROWS     22                         // default: 22
#define MAP_TOP_ROW  1                          // a 0. sor az üzenetsor (default: 1)

// a koordináták 8 bitesek, és két oszlop különbsége (dist2, nézőablak) előjelesen 8 biten is elfér
#if MAP_COLUMNS > 128 || MAP_TOP_ROW + MAP_ROWS > 128
    #error "a térkép túl nagy a 8 bites koordinátákhoz"
#endif

// a logikai képernyő: a térkép fölött az üzenetsor, alatta a státuszsor
#define LOGICAL_SCREEN_ROWS  (MAP_TOP_ROW + MAP_ROWS + 1)     // default: 24

#define IN_PASSAGE  0xfe                        // a lény nem szobában, hanem folyosón áll; a labirintus szobának számít

// a játékos és a szörnyek közös rekordja
struct Creature {
    uint8_t  x;                                 // a pozíció a térképen
    uint8_t  y;
    uint8_t  room;                              // a szoba indexe, amelyben áll, vagy IN_PASSAGE
    uint8_t  strength;                          // 3..31
    uint8_t  experience_level;
    int16_t  hit_points;
    int16_t  max_hit_points;
    uint32_t experience;
};

// a logikai képernyő SECTOR_GRID_SIZE × SECTOR_GRID_SIZE szektorra osztva
#define SECTOR_GRID_SIZE  3                     // default: 3

// a szobák száma: szektoronként egy, a szoba indexe a szektor indexe
#define ROOM_COUNT  (SECTOR_GRID_SIZE * SECTOR_GRID_SIZE)      // default: 9

// a szobaindex 8 bites: a szobák ciklusai uint8_t számlálóval mennek, és a 0xfe és a 0xff szabad marad (IN_PASSAGE, ANY_ROOM)
#if ROOM_COUNT > 254
    #error "túl sok szoba: a szobaindex 8 bites"
#endif

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

    uint8_t  map_characters[MAP_ROWS][MAP_COLUMNS]; // a MAP karakterrétege: a mező valódi jele, a logikai y sora a [y - MAP_TOP_ROW]
    uint8_t  map_flags[MAP_ROWS][MAP_COLUMNS];      // a MAP flagrétege: a mezőflagek (map.h)

    uint8_t  view_window_x;                     // a nézőablak bal szélének logikai oszlopa
    uint8_t  seen_map[MAP_ROWS][MAP_COLUMNS];   // a SCREEN réteg, a logikai y sora a [y - MAP_TOP_ROW]; 7. bit = inverz
};

extern struct GameState game;

// a beállítások bitjei (4.2.5)
#define OPTION_FOLLOW_PASSAGES  0x01            // futásnál a folyosó kanyarjainak követése
#define OPTION_JUMP_TO_RUN_END  0x02            // futás közben nincs rajzolás, csak a végén

// a beállításblokk: a GameState-en kívül, mert a képernyőmodul is használja; alapból minden kikapcsolva (nulla)
struct Options {
    uint8_t flags;                              // OPTION_ bitek
};

extern struct Options options;

#endif
