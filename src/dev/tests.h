#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

// egy önteszt: az i. próbát futtatja, igaz = sikerült
// int, nem bool: az Oscar64 1.32.273 a függvénymutatón át hívott 8 bites visszatérési értéket rossz helyről olvassa
int test_RN(uint16_t i);
int test_rnd(uint16_t i);
int test_ROLL(uint16_t i);
int test_spread(uint16_t i);
int test_saturating_arithmetic(uint16_t i);
int test_format(uint16_t i);
int test_key_decode(uint16_t i);
int test_view_follow(uint16_t i);
int test_prepare_message_text(uint16_t i);
int test_message_chars_fitting_row(uint16_t i);
int test_message_chars_fitting_bar(uint16_t i);

#endif
