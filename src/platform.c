#include "platform.h"

#include <conio.h>
#include <c64/memmap.h>

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

#define SCREEN_RAM   ((volatile uint8_t *)0xC000)
#define CHARSET_RAM  ((volatile uint8_t *)0xC800)

// ideiglenes: a gyári képernyő ($0400) és karakterkészlet (az M01-ben $C000 lesz, saját ASCII karakterkészlettel)
#define SCREEN_TMP  ((volatile uint8_t *)0x0400)

volatile uint8_t frame_count;

// a raszter-IRQ C-ben írt része; az __interrupt menti a fordító zero page regisztereit
__interrupt void irq_body(void)
{
    frame_count++;
}

// ide mutat a $0314: lefuttatja a C részt, nyugtázza a VIC-et, majd a KERNAL-ra ugrik ($EA31: billentyűzet, jiffy óra, kurzor; az M02-02-ig ez olvassa a billentyűket)
__asm irq_stub
{
    jsr irq_body
    lda #$ff
    sta $d019
    jmp $ea31
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

    *(volatile uint8_t *)0xdc0d = 0x7f;     // CIA1 időzítő-IRQ ki
    (void)*(volatile uint8_t *)0xdc0d;      // a függő CIA-IRQ törlése

    *(void **)0x0314 = irq_stub;            // IRQ-vektor
    *(void **)0x0318 = nmi_stub;            // NMI-vektor

    *(volatile uint8_t *)0xd012 = 250;      // a raszter-IRQ sora (a látható rész alatt)
    *(volatile uint8_t *)0xd011 &= 0x7f;    // a sorszám 9. bitje 0
    *(volatile uint8_t *)0xd01a = 0x01;     // raszter-IRQ be
    *(volatile uint8_t *)0xd019 = 0xff;     // a függő VIC-IRQ törlése

    __asm { cli }
}

void platform_exit(void)
{
    __asm { sei }

    *(volatile uint8_t *)0xd01a = 0x00;     // raszter-IRQ ki
    *(volatile uint8_t *)0xd019 = 0xff;     // a függő VIC-IRQ törlése

    *(void **)0x0314 = (void *)0xea31;      // gyári IRQ-vektor
    *(void **)0x0318 = (void *)0xfe47;      // gyári NMI-vektor

    *(volatile uint8_t *)0xdc0d = 0x81;     // CIA1 időzítő-IRQ vissza

    __asm { cli }

    mmap_set(MMAP_ROM);                     // BASIC ROM be
}



void screen_clear(void)
{
    for (uint8_t i = 0; i < 250; i++) {
        SCREEN_TMP[i]       = ' ';
        SCREEN_TMP[i + 250] = ' ';
        SCREEN_TMP[i + 500] = ' ';
        SCREEN_TMP[i + 750] = ' ';
    }
}

void screen_put(uint8_t x, uint8_t y, uint8_t ch)
{
    SCREEN_TMP[x + y * SCREEN_W] = ch;
}



// ideiglenes: a KERNAL GETIN-je (a conio ASCII-ra alakítja);
uint8_t key_get(void)
{
    return getchx();
}



bool disk_exists(const char *name)
{
    return false;
}

bool disk_save(const char *name, const void *data, uint16_t size)
{
    return false;
}

bool disk_load(const char *name, void *data, uint16_t size)
{
    return false;
}



void wait_frame(void)
{
    uint8_t f = frame_count;
    while (frame_count == f) {}
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
