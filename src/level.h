#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>
#include <stdbool.h>

#define AMULET_LEVEL  26                        // ezen a mélységen van az amulett; ez alatt a szörnyek erősebbek (default: 26)

// a keresés egy véletlen nem fantom szobában kezd, és ha ott nincs alkalmas mező, másikban folytatja;
// nem lehet szobaindex, mert a szobatáblában legfeljebb 255 elem van (state.h), így a legnagyobb index 254
#define ANY_ROOM  0xff

// a szoba falain belüli alkalmas mezők közül egy, egyforma eséllyel; hamis, ha nincs ilyen (ANY_ROOM-nál egyik szobában sem)
// room_index: nem fantom szoba vagy ANY_ROOM
bool find_empty_floor(uint8_t room_index, uint8_t *x, uint8_t *y);         // üres padló, labirintusban üres járat: tárgynak, lépcsőnek
bool find_walkable_tile(uint8_t room_index, uint8_t *x, uint8_t *y);       // járható mező szörny nélkül: a játékosnak
// a szint a MAP-ba a game.dungeon_level mélységen: szobák, folyosók, lépcső, a játékos helye (game.player.x, y), titkos folyosó-mezők, arany,
// kezdő szörnyek, tárgyak, amulett; ha a mélység nagyobb a game.max_level-nél, az lesz az új; az előző szint padlón lévő tárgyai
// felszabadulnak; a képernyőhöz nem nyúl, ezért a fejlesztői tesztek is ezt hívják
void generate_level(void);

// új szint: a látott térkép törlése, generate_level, majd a nézőablak a játékosra áll, teljes újrarajzolással
void new_level(void);

// a > és a < parancs: lépcsőn le, illetve fel; lebegve nem lehet; nem visz időt
void go_down_stairs(void);
void go_up_stairs(void);

#endif
