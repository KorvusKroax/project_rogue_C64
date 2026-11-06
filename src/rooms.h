#ifndef ROOMS_H
#define ROOMS_H

#include <stdint.h>

void generate_rooms(void);              // a 9 szoba a 3×3 szektorba, a szint mélysége szerint

uint8_t choose_random_room(void);       // egy véletlen, nem fantom szoba indexe

#endif
