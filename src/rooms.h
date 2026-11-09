#ifndef ROOMS_H
#define ROOMS_H

#include <stdint.h>

#include "state.h"

// a szektorok: az i. szektor sora i / SECTOR_GRID_SIZE, oszlopa i % SECTOR_GRID_SIZE
#define SECTOR_WIDTH         (MAP_COLUMNS / SECTOR_GRID_SIZE)            // az egész osztás lefelé kerekít (default: 80 / 3 = 26)
#define SECTOR_HEIGHT        (LOGICAL_SCREEN_ROWS / SECTOR_GRID_SIZE)    // default: 24 / 3 = 8
#define SECTOR_FIRST_COLUMN  1                                           // a szektorok az 1. oszloptól kezdődnek (default: 1)

#define DARK_ROOM_CHANCE_DIVISOR  10            // a szoba min(mélység - 1, DARK_ROOM_CHANCE_DIVISOR) / DARK_ROOM_CHANCE_DIVISOR eséllyel sötét (default: 10)
#define MAZE_CHANCE_DIVISOR       15            // a sötét szobából 1 / MAZE_CHANCE_DIVISOR eséllyel labirintus lesz (default: 15)

void generate_rooms(void);              // a 9 szoba a 3×3 szektorba, a szint mélysége szerint, és a rajzuk a MAP-ba; a MAP-ot előtte törölni kell (clear_map)

uint8_t choose_random_room(void);       // egy véletlen, nem fantom szoba indexe

void put_passage(uint8_t x, uint8_t y); // folyosó-mező a labirintusnak és a folyosóknak; mélyebben titkos is lehet (üresnek látszik)

#endif
