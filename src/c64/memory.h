#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// memóriatérkép
// $0880-$BFFF  kód, állandó adatok, változók (benne a GameState), verem
//              – a main régió, lásd a #pragma region sort
// $C000-$C3FF  képernyőmemória (VIC 3. bank)
// $C400-$C7FF  szabad
// $C800-$CFFF  saját karakterkészlet
// $D000-$DFFF  I/O
// $E000-$FFFF  KERNAL ROM (mindig bent)

#define SCREEN_RAM   ((volatile uint8_t *)0xC000)
#define CHARSET_RAM  ((volatile uint8_t *)0xC800)

#endif
