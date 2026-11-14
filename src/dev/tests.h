#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdint.h>

// egy önteszt: az i. próbát futtatja, igaz = sikerült
// int, nem bool: az Oscar64 1.32.273 a függvénymutatón át hívott 8 bites visszatérési értéket rossz helyről olvassa
int test_RN(uint16_t i);
int test_rnd(uint16_t i);
int test_ROLL(uint16_t i);
int test_spread(uint16_t i);
int test_saturating_arithmetic(uint16_t i);
int test_format(uint16_t i);
int test_convert_text_to_number(uint16_t i);
int test_key_decode(uint16_t i);
int test_view_follow(uint16_t i);
int test_prepare_message_text(uint16_t i);
int test_message_chars_fitting_row(uint16_t i);
int test_message_chars_fitting_bar(uint16_t i);
int test_map_flags(uint16_t i);
int test_diagonal_step(uint16_t i);
int test_neighbor_visibility(uint16_t i);
int test_careful_run(uint16_t i);

// szint-önteszt: billentyűre indul, mert hosszú (VICE-ben warp módban)
#define LEVEL_TEST_LEVEL_COUNT  1000            // ennyi véletlen szint, rögzített magokkal
#define LEVEL_TEST_DEPTH_COUNT  4               // ennyi mélységen, felváltva (test_level.c)

int test_level_generation(uint16_t i);          // az i. szint generálása és ellenőrzése: szobák, labirintusok, összekötés
bool show_level_room_ratios(void);              // mélységenként a sötét szobák és a labirintusok várt és kapott száma, a test_level_generation után; igaz, ha mind a tűrésen belül (leállított futásnál nem ítél, igazat ad)
bool show_level_generation_times(void);         // mélységenként a leglassabb szintgenerálás képkockában, a test_level_generation után; igaz, ha mind a 2 másodperces kereten belül

#endif
