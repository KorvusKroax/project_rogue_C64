#ifndef EXPLORE_H
#define EXPLORE_H

#include <stdint.h>
#include <stdbool.h>

void enter_room(uint8_t x, uint8_t y);          // a játékos az x, y mező szobájába lép: világos szobánál az egész szoba kirajzolódik
void leave_room(uint8_t x, uint8_t y);          // a játékos ajtóról az x, y folyosó-mezőre lép: sötét szobánál a szoba padlója törlődik

void look_around(void);                         // minden akció előtt: a játékos körüli 3×3 kirajzolása, végül a játékos jele
void forget_previous_position(void);            // új szinten, az enter_room után: a look_around nem töröl a régi szint helyén (J20)

uint8_t calculate_character_left_behind(void);  // a jel, amely a játékos mezőjére kerül, amikor elhagyja

// a from_x, from_y mezőről a szomszédos x, y mező látszik-e (a térképen belül): folyosó és szoba között csak ajtón át, sarkon át átlósan nem
bool is_neighbor_visible(uint8_t from_x, uint8_t from_y, uint8_t x, uint8_t y);

#endif
