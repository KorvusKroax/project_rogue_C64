#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

// a buffer legalább max_length + 1 bájtos; a mező üresen indul, és max_length + 1 cellás, mert a kurzor a teli mezőben is a szöveg után áll
bool input_text(char *buffer, uint8_t max_length, uint8_t last_row);    // a kurzortól, legfeljebb a last_row sor végéig; RETURN: igaz, ESC: hamis és üres szöveg
bool ask_text(const char *prompt, char *buffer, uint8_t max_length);    // kérdés a sávba a mező helyével (message_prompt_with_input_space), utána input_text; utána a sáv üres
uint8_t ask_key(const char *prompt);                                    // kérdés a sávba, egy leütés kódját adja, a válasz látszik; ESC-re törli a sávot

#endif
