#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

// a buffer legalább max_length + 1 bájtos
bool input_text(char *buffer, uint8_t max_length, uint8_t last_row);    // a kurzortól, legfeljebb a last_row sor végéig; RETURN: igaz, ESC: hamis és üres szöveg
bool ask_text(const char *prompt, char *buffer, uint8_t max_length);    // kérdés a sávba, utána input_text; ESC-re törli a sávot
uint8_t ask_key(const char *prompt);                                    // kérdés a sávba, egy leütés kódját adja, a válasz látszik; ESC-re törli a sávot

#endif
