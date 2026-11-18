#ifndef EXPLORE_H
#define EXPLORE_H

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

// a játékos látja-e a szörnyet: a láthatatlant nem; a szomszédosat (átlósan sarkon át nem), a távolabbit csak a saját világos szobájában (see_monst)
bool is_monster_visible(const struct Monster *monster);

// a mező jele, ahogy a játékos látja: a látható szörny álcája, különben a MAP jele
uint8_t get_visible_character(uint8_t x, uint8_t y);

void enter_room(uint8_t x, uint8_t y);          // a játékos az x, y mező szobájába lép: a szoba szörnyei felébredhetnek; világos szobánál az egész szoba kirajzolódik, a látható szörnyekkel
void leave_room(uint8_t x, uint8_t y);          // a játékos ajtóról az x, y folyosó-mezőre lép: sötét szobánál a szoba padlója törlődik, a szörnyek jele helyére padló (ajtó) kerül; a szoba szörnyei felébredhetnek

void look_around(void);                         // minden akció előtt: a játékos körüli 3×3 kirajzolása, a látott szomszédos szörnyek felébredhetnek; végül a játékos jele
void forget_previous_position(void);            // új szinten, az enter_room után: a look_around nem töröl a régi szint helyén (J20)

uint8_t calculate_character_left_behind(void);

// keresés: a játékos körüli 8 mező közül a rejtettek esélyre előkerülnek; titkos ajtónál üzenet
void search_around(void);

// a rejtett x, y mező esélyre előkerül (a jele ajtó, illetve folyosó lesz, és látható); igaz, ha előkerült
bool try_to_find_hidden_tile(uint8_t x, uint8_t y);  // a jel, amely a játékos mezőjére kerül, amikor elhagyja

// a from_x, from_y mezőről a szomszédos x, y mező látszik-e (a térképen belül): folyosó és szoba között csak ajtón át, sarkon át átlósan nem
bool is_neighbor_visible(uint8_t from_x, uint8_t from_y, uint8_t x, uint8_t y);

#endif
