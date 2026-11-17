#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdbool.h>
#include <stdint.h>

// egy önteszt a teszttáblában
struct DevTest {
    const char *name;
    int (*fn)(uint16_t i);  // az i. próba, igaz = sikerült (int: lásd tests.h)
    uint16_t count;         // hány próba (1 = egyszeri teszt)
};

#define TEST_COUNT(tests)  (sizeof(tests) / sizeof((tests)[0]))

void init_test_program(void);       // a tesztprogram elején: platform, RNG és seed, mint a játékban; üres képernyő, a kurzor a bal felső sarokban
void end_test_program(bool all_ok); // a tesztprogram végén: „Done” az utolsó sorba, az eredmény a VICE-nek (-debugcart: 0 = mind sikerült, 1 = nem), majd megáll; a képernyő marad

// a tesztek egymás után, soronként a név és az eredmény; a többpróbás teszt ESC-re leáll; igaz, ha mind sikerült
bool run_test_table(const struct DevTest *tests, uint8_t test_count);
void put_failed_test_names(const char *prefix);     // az utolsó run_test_table hibás tesztjei soronként, a prefix után

#endif
