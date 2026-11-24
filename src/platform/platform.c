#include "platform.h"

#include <c64/cia.h>
#include <c64/memmap.h>
#include <c64/vic.h>

#include "disk.h"
#include "key.h"
#include "screen.h"

// memóriatérkép
// $0880-$BFFF  kód, állandó adatok, változók (benne a GameState), verem
//              – a main régió, lásd a #pragma region sort
// $C000-$C3FF  képernyőmemória (VIC 3. bank)
// $C400-$C7FF  szabad; induláskor a szövegek betöltésének puffere
// $C800-$CFFF  saját karakterkészlet
// $D000-$DFFF  I/O, alatta szövegek
// $E000-$FFF9  KERNAL ROM, alatta szövegek
//              – a texts régió, lásd a #pragma region sort
// $FFFA-$FFFF  KERNAL ROM, alatta NMI-, RESET- és IRQ vektorok

#pragma region(main, 0x0880, 0xc000, , , {code, data, bss, heap, stack})
#pragma heapsize(0)

// a texts szakasz (texts.c) a lemezen külön fájl (az 1-es overlay), a neve a régió neve; induláskor a load_texts tölti be
#pragma region(texts, 0xd000, 0xfffa, , 1, {texts})
#pragma overlay(texts, 1)

extern void texts_section_start, texts_section_end;

#define TEXTS_FILE_NAME       "TEXTS"               // a régió neve, PETSCII-ben nagybetűvel
#define TEXT_LOAD_BUFFER      ((char *)0xc400)
#define TEXT_LOAD_CHUNK_SIZE  256
#define LOADING_ROW           0                     // a szövegek betöltésének állapotsora, mint a „saving...”-é (10.7)

#if TEXT_LOAD_CHUNK_SIZE > 0x400
    #error "a szövegek betöltésének puffere nem fér el a $C400-$C7FF területen"
#endif

// a fordító becsült veremmélysége a .map fájl STACK sorában (a régió vége − a sor kezdete); mérve 194 bájt volt a dev buildben
#pragma stacksize(512)

#define IRQ_VECTOR            (*(void **)0x0314)
#define NMI_VECTOR            (*(void **)0x0318)
#define RAM_NMI_VECTOR        (*(void **)0xfffa)            // a KERNAL ROM alatt: kilapozott ROM mellett ezt használja a processzor
#define JIFFY_CLOCK          ((volatile uint8_t *)0x00a0)  // 3 bájt
#define VIC_RASTER_INTERRUPT  0x01

// a processzor $01 portja: a ROM-ok és az I/O be- és kilapozása; a __memmap jelzi a fordítónak, hogy az írása
// a memóriatérképet változtatja (mint az Oscar64 mmap_set-jében)
#define MEMORY_CONFIGURATION  (*(volatile __memmap uint8_t *)0x01)

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

// ide mutat a $0318, kilapozott ROM mellett a RAM-beli $FFFA: a RESTORE (NMI) nem csinál semmit
__asm restore_key_handler
{
    rti
}

// mint a memcpy, a ROM-ok és az I/O alá is; a php és a plp miatt tiltott megszakítás mellett hívva sem engedélyezi
static void copy_to_memory_under_roms(char *destination, const char *source, uint16_t count)
{
    __asm {
        php
        sei
    }
    uint8_t memory_configuration = MEMORY_CONFIGURATION;
    MEMORY_CONFIGURATION = MMAP_RAM;

    for (uint16_t position = 0; position < count; position++) {
        destination[position] = source[position];
    }

    MEMORY_CONFIGURATION = memory_configuration;
    __asm {
        plp
    }
}

// a fájl a betöltési címmel kezdődik, utána a texts szakasz; a LOAD a $D000-$DFFF alá nem írhat, mert ott az I/O látszik, ezért
// darabonként a pufferbe olvas, és onnan másol a helyére; igaz, ha a fájlban pontosan a szakasz volt, és hiba nélkül beolvasódott
static bool load_texts(void)
{
    char *destination = (char *)&texts_section_start;
    uint16_t remaining = (char *)&texts_section_end - destination;
    bool is_loaded = false;

    // szöveg nélküli programban (pl. egy tesztprogramban) a szakasz üres, és a fordító betöltési cím nélküli fájlt ír a lemezre
    // (README, Oscar64-hibák, 13.)
    if (remaining == 0) return true;

    if (open_file_for_reading(TEXTS_FILE_NAME, get_game_device())) {
        uint16_t load_address = 0;
        if (read_file((char *)&load_address, sizeof(load_address)) == sizeof(load_address) && load_address == (uint16_t)destination) {
            while (remaining > 0) {
                uint16_t chunk_size = remaining < TEXT_LOAD_CHUNK_SIZE ? remaining : TEXT_LOAD_CHUNK_SIZE;
                uint16_t count = read_file(TEXT_LOAD_BUFFER, chunk_size);
                copy_to_memory_under_roms(destination, TEXT_LOAD_BUFFER, count);
                destination += count;
                remaining -= count;
                if (count < chunk_size) break;
            }
            is_loaded = remaining == 0 && is_file_end_reached();
        }
        close_file();
    }
    return is_loaded;
}

// állapotjelzés a LOADING_ROW sorában, nem üzenet: a sor többi része üres
static void show_loading_status(const char *text)
{
    set_screen_cursor(0, LOADING_ROW);
    put_text(text);
    clear_to_row_end();
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
    RAM_NMI_VECTOR = restore_key_handler;   // a ROM-ra írás az alatta lévő RAM-ba kerül; a screen_init előtt, mert a ROM-os karakterkészlet másolása is kilapozza a KERNAL-t

    vic.raster = RASTER_INTERRUPT_ROW;
    vic.ctrl1 &= 0x7f;                      // a sorszám 9. bitje 0
    vic.intr_enable = VIC_RASTER_INTERRUPT;
    vic.intr_ctrl = 0xff;                   // a függő VIC-IRQ törlése

    screen_init();

    __asm { cli }

    // ha a fájl nem olvasható be, vagy csak hiányosan (nincs a lemezen, az egység nem válaszol), szóközre újra
    show_loading_status("loading...");
    while (!load_texts()) {
        show_loading_status("insert the game disk and press space");
        while (wait_for_key() != ' ') {}
        show_loading_status("loading...");
    }
    show_loading_status("");
}

// ideiglenes: az E13 szerinti végleges változat (az N leütésekor) az M10-05-ben jön
uint32_t calculate_seed(void)
{
    uint32_t seed = ((uint32_t)(JIFFY_CLOCK[0]) << 16) | ((uint32_t)(JIFFY_CLOCK[1]) << 8) | JIFFY_CLOCK[2];
    seed |= (uint32_t)(vic.raster) << 24;
    seed ^= cia1.ta;
    return seed;
}

// a jiffy óra a legnagyobb helyiértékkel kezdődik: $A1 a felső, $A2 az alsó bájt; ha olvasás közben átfordul, újra
uint16_t get_frame_count_16(void)
{
    uint8_t high;
    uint8_t low;

    do {
        high = JIFFY_CLOCK[1];
        low = JIFFY_CLOCK[2];
    } while (high != JIFFY_CLOCK[1]);

    return ((uint16_t)high << 8) | low;
}

void wait_for_next_frame(void)
{
    uint8_t start_frame = frame_count;
    while (frame_count == start_frame) {}
}

// a hívó lapozza ki a ROM-okat
static char *copy_characters(char *destination, const char *text, uint8_t max_length)
{
    uint8_t length = 0;
    while (length < max_length && text[length]) {
        destination[length] = text[length];
        length++;
    }

    destination[length] = 0;
    return destination + length;
}

// a php és a plp miatt tiltott megszakítás mellett hívva sem engedélyezi
char *copy_text(char *destination, const char *text, uint8_t max_length)
{
    __asm {
        php
        sei
    }
    uint8_t memory_configuration = MEMORY_CONFIGURATION;
    MEMORY_CONFIGURATION = MMAP_RAM;

    char *end = copy_characters(destination, text, max_length);

    MEMORY_CONFIGURATION = memory_configuration;
    __asm {
        plp
    }
    return end;
}

// a mutató kiolvasása és a szöveg másolása egy lapozásban
char *copy_text_from_table(char *destination, const char *const *table, uint8_t index, uint8_t max_length)
{
    __asm {
        php
        sei
    }
    uint8_t memory_configuration = MEMORY_CONFIGURATION;
    MEMORY_CONFIGURATION = MMAP_RAM;

    char *end = copy_characters(destination, table[index], max_length);

    MEMORY_CONFIGURATION = memory_configuration;
    __asm {
        plp
    }
    return end;
}

const char *get_text_from_table(const char *const *table, uint8_t index)
{
    __asm {
        php
        sei
    }
    uint8_t memory_configuration = MEMORY_CONFIGURATION;
    MEMORY_CONFIGURATION = MMAP_RAM;

    const char *text = table[index];

    MEMORY_CONFIGURATION = memory_configuration;
    __asm {
        plp
    }
    return text;
}
