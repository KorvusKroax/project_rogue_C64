#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>
#include <stdbool.h>

// a keresés egy véletlen nem fantom szobában kezd, és ha ott nincs alkalmas mező, másikban folytatja;
// nem lehet szobaindex, mert legfeljebb 255 szoba van (state.h), így a legnagyobb index 254
#define ANY_ROOM  0xff

// a szoba falain belüli alkalmas mezők közül egy, egyforma eséllyel; hamis, ha nincs ilyen (ANY_ROOM-nál egyik szobában sem)
// room_index: nem fantom szoba vagy ANY_ROOM
bool find_empty_floor(uint8_t room_index, uint8_t *x, uint8_t *y);         // üres padló, labirintusban üres járat: tárgynak, lépcsőnek
bool find_walkable_tile(uint8_t room_index, uint8_t *x, uint8_t *y);       // járható mező szörny nélkül: a játékosnak

#endif
