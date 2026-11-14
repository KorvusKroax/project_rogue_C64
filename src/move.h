#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include <stdbool.h>

// a lépés a from mezőről a szomszédos to mezőre a sarkok miatt megengedett-e: átlósan csak akkor, ha a sarok mindkét mezője járható
// (így ajtóra átlósan sem lehet lépni, sem róla le); egyenesen mindig
bool is_diagonal_step_allowed(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);

// a játékos lépése a step_x, step_y irányba (-1, 0, 1); igaz, ha időt vitt
bool move_player(int8_t step_x, int8_t step_y);

#endif
