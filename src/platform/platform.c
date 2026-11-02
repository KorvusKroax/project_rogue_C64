#include "platform.h"

#include <c64/cia.h>
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

#define IRQ_VECTOR            (*(void **)0x0314)
#define NMI_VECTOR            (*(void **)0x0318)
#define KERNAL_IRQ_HANDLER    ((void *)0xea31)
#define KERNAL_NMI_HANDLER    ((void *)0xfe47)
#define JIFFY_CLOCK           ((volatile uint8_t *)0x00a0)  // 3 bájt
#define BASIC_STRING_STACK    ((volatile uint8_t *)0x0016)  // 3 bájt
#define RASTER_INTERRUPT_ROW  250                           // a látható rész alatt
#define VIC_RASTER_INTERRUPT  0x01

volatile uint8_t frame_count;

// ide mutat a $0314: lépteti a képkocka-számlálót, lefuttatja a billentyűkezelést, nyugtázza a VIC-et, lépteti a jiffy órát, majd a KERNAL-on át visszatér
__asm raster_interrupt_handler
{
    inc frame_count
    jsr key_irq
    lda #$ff
    sta $d019
    jsr $ffea   // UDTIM: jiffy óra
    jmp $ea81  // a verem visszaállítása, RTI
}

// ide mutat a $0318: a RESTORE (NMI) nem csinál semmit
__asm restore_key_handler
{
    rti
}

void platform_init(void)
{
    mmap_set(MMAP_NO_BASIC);

    __asm { sei }

    key_init();

    cia1.icr = 0x7f;                        // CIA1 időzítő-IRQ ki
    (void)(cia1.icr);                       // a függő CIA-IRQ törlése

    IRQ_VECTOR = raster_interrupt_handler;
    NMI_VECTOR = restore_key_handler;

    vic.raster = RASTER_INTERRUPT_ROW;
    vic.ctrl1 &= 0x7f;                      // a sorszám 9. bitje 0
    vic.intr_enable = VIC_RASTER_INTERRUPT;
    vic.intr_ctrl = 0xff;                   // a függő VIC-IRQ törlése

    screen_init();

    __asm { cli }
}

void platform_exit(void)
{
    wait_for_keys_released();

    __asm { sei }

    vic.intr_enable = 0;                    // raszter-IRQ ki
    vic.intr_ctrl = 0xff;                   // a függő VIC-IRQ törlése

    IRQ_VECTOR = KERNAL_IRQ_HANDLER;
    NMI_VECTOR = KERNAL_NMI_HANDLER;

    vic_setbank(0);
    __asm { jsr $ff81 }                   // CINT: VIC, színek, képernyőtörlés, CIA1 időzítő-IRQ

    __asm { cli }

    mmap_set(MMAP_ROM);                     // BASIC ROM be

    // a BASIC szövegverem-mutatói: a fordító zero page-e felülírja őket
    BASIC_STRING_STACK[0] = 0x19;
    BASIC_STRING_STACK[1] = 0x16;
    BASIC_STRING_STACK[2] = 0x00;
}

// ideiglenes: az E13 szerinti végleges változat (az N leütésekor) az M10-05-ben jön
uint32_t calculate_seed(void)
{
    uint32_t seed = ((uint32_t)(JIFFY_CLOCK[0]) << 16) | ((uint32_t)(JIFFY_CLOCK[1]) << 8) | JIFFY_CLOCK[2];
    seed |= (uint32_t)(vic.raster) << 24;
    seed ^= cia1.ta;
    return seed;
}

void wait_for_next_frame(void)
{
    uint8_t start_frame = frame_count;
    while (frame_count == start_frame) {}
}
