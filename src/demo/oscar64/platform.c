#include <conio.h>
#include "../platform.h"
#include "../rng.h"

void clear_screen(void)
{
    clrscr();
}

uint16_t RN_asm(void)
{
    return __asm {
        lda #0              // res (__tmp+0..3) = 0
        sta __tmp + 0
        sta __tmp + 1
        sta __tmp + 2
        sta __tmp + 3
        lda #$65            // mul (__tmp+4..5) = 11109 = $2B65
        sta __tmp + 4
        lda #$2B
        sta __tmp + 5

        ldy #14             // $2B65 legfelső 1-es bitje a 13., 14 kör elég
    loop:
        lsr __tmp + 5       // a szorzó legalsó bitje a carry-be
        ror __tmp + 4
        bcc skip
        clc                 // res += seed
        lda __tmp + 0
        adc seed + 0
        sta __tmp + 0
        lda __tmp + 1
        adc seed + 1
        sta __tmp + 1
        lda __tmp + 2
        adc seed + 2
        sta __tmp + 2
        lda __tmp + 3
        adc seed + 3
        sta __tmp + 3
    skip:
        asl seed + 0        // szorzandó << 1 (a seed-et a végén úgyis felülírjuk)
        rol seed + 1
        rol seed + 2
        rol seed + 3
        dey
        bne loop

        clc                 // seed = res + 13849 ($3619)
        lda __tmp + 0
        adc #$19
        sta seed + 0
        lda __tmp + 1
        adc #$36
        sta seed + 1
        lda __tmp + 2
        adc #0
        sta seed + 2
        sta accu + 0        // visszatérési érték: seed >> 16
        lda __tmp + 3
        adc #0
        sta seed + 3
        sta accu + 1
    };
}
