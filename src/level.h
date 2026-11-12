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

// a szint a MAP-ba a game.dungeon_level mélységen: szobák, folyosók, lépcső, a játékos helye (game.player.x, y), titkos folyosó-mezők;
// a képernyőhöz nem nyúl, ezért a fejlesztői tesztek is ezt hívják
void generate_level(void);

// új szint: a látott térkép törlése, generate_level, majd a nézőablak a játékosra áll, teljes újrarajzolással
void new_level(void);

#endif
