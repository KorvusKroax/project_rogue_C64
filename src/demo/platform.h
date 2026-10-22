#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

// Fordítónként külön megvalósítás:
//   oscar64/platform.c                       – Oscar64
//   llvm-mos/platform.c + llvm-mos/rn.s      – llvm-mos

void clear_screen(void);
uint16_t RN_asm(void);          // ugyanaz, mint az RN(), assemblyben

// llvm-mos: az asm-ből név szerint elért C-változó ne essen ki az optimalizáláskor
#if defined(__mos__)
#define ASM_USED __attribute__((used))
#else
#define ASM_USED
#endif

#endif
