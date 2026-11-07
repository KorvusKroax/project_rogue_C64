#ifndef STATUS_H
#define STATUS_H

#include "platform/screen.h"

#define STATUS_ROW_COUNT  2                                     // a státuszsor sorai
#define STATUS_ROW        (SCREEN_HEIGHT - STATUS_ROW_COUNT)    // a státuszsor első képernyősora, a képernyő alján; a második a STATUS_ROW + 1

void draw_status(void);     // a két sor kiírása a régi fölé, a sorok végét törölve; a 40. oszlopon túli rész levágódik

#endif
