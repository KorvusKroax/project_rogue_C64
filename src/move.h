#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include <stdbool.h>

// a lépés a from mezőről a szomszédos to mezőre a sarkok miatt megengedett-e: átlósan csak akkor, ha a sarok mindkét mezője járható
// (így ajtóra átlósan sem lehet lépni, sem róla le); egyenesen mindig
bool is_diagonal_step_allowed(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y);

// a játékos lépése a step_x, step_y irányba (-1, 0, 1); igaz, ha időt vitt
bool move_player(int8_t step_x, int8_t step_y);

// futás a step_x, step_y irányba: az indítás nem visz időt, a lépéseket a take_run_step teszi meg;
// az óvatos (CTRL) futás minden lépés után megáll, ha valami érdekes van a játékos mellett
void start_running(int8_t step_x, int8_t step_y, bool is_careful);
void stop_running(void);
bool is_running(void);

// a futás következő lépése; ha nem sikerül, a futás véget ér; igaz, ha időt vitt
bool take_run_step(void);

// az x, y mezőn álló, step_x, step_y irányba futó játékos mellett, a látható mezők közül a hátsó oldalt kihagyva:
// ortogonálisan ajtó, ortogonálisan egynél több folyosó-mező, vagy bármi, ami nem padló, fal, kőzet, folyosó vagy ajtó
bool is_something_interesting_beside(uint8_t x, uint8_t y, int8_t step_x, int8_t step_y);

#endif
