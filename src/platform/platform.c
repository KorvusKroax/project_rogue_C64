#include "platform.h"

#include <c64/memmap.h>
#include <c64/vic.h>

#include "key.h"
#include "screen.h"

// memóriatérkép
// $0880-$BFFF  kód, állandó adatok, változók (benne a GameState), verem
//              – a main régió, lásd a #pragma region sort
// $C000-$C3FF  képernyőmemória (VIC 3. bank)
// $C400-$C7FF  szabad
// $C800-$CFFF  saját karakterkészlet
// $D000-$DFFF  I/O
// $E000-$FFFF  KERNAL ROM (mindig bent)

#pragma region(main, 0x0880, 0xc000, , , {code, data, bss, heap, stack})
#pragma heapsize(0)

volatile uint8_t frame_count;

// ide mutat a $0314: lépteti a képkocka-számlálót, lefuttatja a billentyűkezelést, nyugtázza a VIC-et, lépteti a jiffy órát, majd a KERNAL-on át visszatér
__asm irq_stub
{
    inc frame_count
    jsr key_irq
    lda #$ff
    sta $d019
    jsr $ffea   // UDTIM: jiffy óra
    jmp $ea81  // a verem visszaállítása, RTI
}

// ide mutat a $0318: a RESTORE (NMI) nem csinál semmit
__asm nmi_stub
{
    rti
}

void platform_init(void)
{
    mmap_set(MMAP_NO_BASIC);

    __asm { sei }

    key_init();

    *(volatile uint8_t *)0xdc0d = 0x7f;     // CIA1 időzítő-IRQ ki
    (void)*(volatile uint8_t *)0xdc0d;      // a függő CIA-IRQ törlése

    *(void **)0x0314 = irq_stub;            // IRQ-vektor
    *(void **)0x0318 = nmi_stub;            // NMI-vektor

    *(volatile uint8_t *)0xd012 = 250;      // a raszter-IRQ sora (a látható rész alatt)
    *(volatile uint8_t *)0xd011 &= 0x7f;    // a sorszám 9. bitje 0
    *(volatile uint8_t *)0xd01a = 0x01;     // raszter-IRQ be
    *(volatile uint8_t *)0xd019 = 0xff;     // a függő VIC-IRQ törlése

    screen_init();

    __asm { cli }
}

void platform_exit(void)
{
    key_wait_released();

    __asm { sei }

    *(volatile uint8_t *)0xd01a = 0x00;     // raszter-IRQ ki
    *(volatile uint8_t *)0xd019 = 0xff;     // a függő VIC-IRQ törlése

    *(void **)0x0314 = (void *)0xea31;      // gyári IRQ-vektor
    *(void **)0x0318 = (void *)0xfe47;      // gyári NMI-vektor

    vic_setbank(0);
    __asm { jsr $ff81 }                   // CINT: VIC, színek, képernyőtörlés, CIA1 időzítő-IRQ

    __asm { cli }

    mmap_set(MMAP_ROM);                     // BASIC ROM be

    // a BASIC szövegverem-mutatói: a fordító zero page-e felülírja őket
    *(volatile uint8_t *)0x16 = 0x19;
    *(volatile uint8_t *)0x17 = 0x16;
    *(volatile uint8_t *)0x18 = 0x00;
}

// ideiglenes: az E13 szerinti végleges változat (az N leütésekor) az M10-05-ben jön
uint32_t platform_seed(void)
{
    volatile uint8_t *jiffy = (volatile uint8_t *)0x00a0; // KERNAL jiffy óra, 3 bájt

    uint32_t s = ((uint32_t)jiffy[0] << 16) | ((uint32_t)jiffy[1] << 8) | jiffy[2];
    s |= (uint32_t)*(volatile uint8_t *)0xd012 << 24;    // raszter-sor
    s ^= *(volatile uint8_t *)0xdc04;                    // CIA1 A időzítő, alsó bájt
    s ^= (uint32_t)*(volatile uint8_t *)0xdc05 << 8;     // CIA1 A időzítő, felső bájt
    return s;
}

void wait_frame(void)
{
    uint8_t f = frame_count;
    while (frame_count == f) {}
}
