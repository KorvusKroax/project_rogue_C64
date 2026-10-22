#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__OSCAR64C__)
#include <conio.h>
#elif defined(__mos__)
#include <cbm.h>
static void clrscr(void) { cbm_k_chrout(147); }   // PETSCII CLR, mint az Oscar64 clrscr()-je
#endif

#define SCREEN ((char*) 0x0400)

#if defined(__mos__)
#define ASM_USED __attribute__((used))   // llvm-mos: az asm-ből név szerint elért változó ne essen ki
#else
#define ASM_USED
#endif

ASM_USED uint32_t  seed = 111;

uint16_t  RN(void)
{
    seed = seed * 11109 + 13849;
    return (uint16_t)(seed >> 16);
}

#if defined(__mos__)
// az llvm-mos-os RN__asm munkaterülete (az Oscar64 __tmp-je helyett);
// fájlszinten, hogy az asm név szerint elérje
ASM_USED uint8_t res[4], mul[2];
#endif

uint16_t RN__asm(void)
{
#if defined(__OSCAR64C__)
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
#elif defined(__mos__)
    uint8_t lo, hi;
    __asm__ volatile (
        "lda #0\n"              // res = 0
        "sta res\n"
        "sta res+1\n"
        "sta res+2\n"
        "sta res+3\n"
        "lda #$65\n"            // mul = 11109 = $2B65
        "sta mul\n"
        "lda #$2B\n"
        "sta mul+1\n"

        "ldy #14\n"             // $2B65 legfelső 1-es bitje a 13., 14 kör elég
    ".Lloop%=:\n"
        "lsr mul+1\n"           // a szorzó legalsó bitje a carry-be
        "ror mul\n"
        "bcc .Lskip%=\n"
        "clc\n"                 // res += seed
        "lda res\n"
        "adc seed\n"
        "sta res\n"
        "lda res+1\n"
        "adc seed+1\n"
        "sta res+1\n"
        "lda res+2\n"
        "adc seed+2\n"
        "sta res+2\n"
        "lda res+3\n"
        "adc seed+3\n"
        "sta res+3\n"
    ".Lskip%=:\n"
        "asl seed\n"            // szorzandó << 1 (a seed-et a végén úgyis felülírjuk)
        "rol seed+1\n"
        "rol seed+2\n"
        "rol seed+3\n"
        "dey\n"
        "bne .Lloop%=\n"

        "clc\n"                 // seed = res + 13849 ($3619)
        "lda res\n"
        "adc #$19\n"
        "sta seed\n"
        "lda res+1\n"
        "adc #$36\n"
        "sta seed+1\n"
        "lda res+2\n"
        "adc #0\n"
        "sta seed+2\n"
        "tax\n"                 // X = (seed >> 16) alsó bájtja
        "lda res+3\n"
        "adc #0\n"
        "sta seed+3\n"          // A = (seed >> 16) felső bájtja
        : "=x"(lo), "=a"(hi)
        :
        : "y", "c", "v", "memory");
    return (uint16_t)hi << 8 | lo;
#else
    return 0;   // PC-s tesztbuild: nincs KERNAL
#endif
}

int rnd(int n)
{
    return n == 0 ? 0 : RN__asm() % abs(n);
}

void draw_room(int x, int y, int w, int h) // w and h means with walls
{
    int index = x + y * 40;
    for (int i = 0; i < w; i++) { // 67 = "-"
        SCREEN[index+i] = 67;
        SCREEN[index+i+(h-1)*40] = 67;
    }
    for (int i = 1; i < h-1; i++) { // 66 = "|"
        SCREEN[index+i*40] = 66;
        SCREEN[index+i*40+(w-1)] = 66;
    }
}

int main(void)
{
    clrscr();

    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, rnd(100));
    }

    draw_room(12, 3, 10, 5);

    return 0;
}
