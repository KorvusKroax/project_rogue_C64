#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define MESSAGE_ROW  0      // az üzenetsáv első képernyősora; a második a MESSAGE_ROW + 1

void message(const char *text);     // a sávba, az eddigi szöveg után; ha nem fér el, --More--, és szóközre vár
void message_clear(void);           // a sáv törlése; a második sor helyén visszajön a térkép

uint8_t calculate_message_chars_fitting_row(const char *text, uint8_t width);   // ennyi karakter kerül a szövegből egy width széles sorba; nem rajzol
uint8_t calculate_message_chars_fitting_bar(const char *text);                  // ennyi karakter fér a sávba úgy, hogy a --More-- is elférjen; nem rajzol

#endif
