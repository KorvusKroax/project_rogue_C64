#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define MESSAGE_ROW  0      // az üzenetsáv első képernyősora; a második a MESSAGE_ROW + 1

void message_show(const char *text);    // a sáv tartalma a szöveg, szótördeléssel; ami két sorba nem fér, levágódik
void message_clear(void);               // a sáv törlése; a második sor helyén visszajön a térkép

uint8_t message_line_length(const char *text);  // ennyi karakter kerül a szövegből egy sorba; nem rajzol

#endif
