#ifndef ROOMS_H
#define ROOMS_H

#include <stdint.h>

void generate_rooms(void);              // a 9 szoba a 3×3 szektorba, a szint mélysége szerint, és a rajzuk a MAP-ba; a MAP-ot előtte törölni kell (clear_map)

uint8_t choose_random_room(void);       // egy véletlen, nem fantom szoba indexe

#endif
