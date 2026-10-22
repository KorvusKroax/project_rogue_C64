#include <cbm.h>
#include "../platform.h"

// az RN_asm a rn.s fájlban van

void clear_screen(void)
{
    cbm_k_chrout(147);      // PETSCII CLR, mint az Oscar64 clrscr()-je
}
