#include "platform.h"

#include <conio.h>
#include <c64/memmap.h>
#include <c64/vic.h>

#include "charset.h"
#include "num.h"

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
#define COLOR_RAM    ((volatile uint8_t *)0xD800)

#define CIA1_PORT_A  (*(volatile uint8_t *)0xdc00)  // mátrix: oszlopkiválasztás (0 = kiválasztva)
#define CIA1_PORT_B  (*(volatile uint8_t *)0xdc01)  // mátrix: sorolvasás (0 = lenyomva)

volatile uint8_t frame_count;

volatile uint8_t key_matrix[KEY_COLUMNS];
volatile uint8_t key_modifiers;
volatile uint8_t key_press_code;
volatile uint8_t key_press_count;

static uint8_t key_counted[KEY_COLUMNS];    // a már leütésnek számolt, még lenyomott billentyűk

// a billentyűk kódja mátrixhely szerint (oszlop * 8 + sor), oszloponként egy sor; 0 = nincs kódja
static const uint8_t key_codes_plain[KEY_COLUMNS * 8] = {
    KEY_DELETE, KEY_RETURN, 0, 0, 0, 0, 0, 0,          // törlés, RETURN, kurzor, F-billentyűk
    '3', 'w', 'a', '4', 'z', 's', 'e', 0,
    '5', 'r', 'd', '6', 'c', 'f', 't', 'x',
    '7', 'y', 'g', '8', 'b', 'h', 'u', 'v',
    '9', 'i', 'j', '0', 'm', 'k', 'o', 'n',
    '+', 'p', 'l', '-', '.', ':', '@', ',',
    '\\', '*', ';', 0, 0, '=', '^', '/',               // £, *, ;, CLR/HOME, -, =, felfelé nyíl, /
    '1', KEY_ESCAPE, 0, '2', ' ', 0, 'q', KEY_ESCAPE,  // 1, balra nyíl, -, 2, SPACE, -, Q, RUN/STOP
};

static const uint8_t key_codes_shifted[KEY_COLUMNS * 8] = {
    KEY_DELETE, KEY_RETURN, 0, 0, 0, 0, 0, 0,
    '#', 'W', 'A', '$', 'Z', 'S', 'E', 0,
    '%', 'R', 'D', '&', 'C', 'F', 'T', 'X',
    '\'', 'Y', 'G', '(', 'B', 'H', 'U', 'V',
    ')', 'I', 'J', 0, 'M', 'K', 'O', 'N',
    0, 'P', 'L', 0, '>', '[', 0, '<',
    '|', 0, ']', 0, 0, 0, 0, '?',
    '!', KEY_ESCAPE, 0, '"', ' ', 0, 'Q', KEY_ESCAPE,
};

static uint8_t cursor_x, cursor_y;

static __forceinline uint8_t key_code_at(uint8_t position, uint8_t modifiers)
{
    if (modifiers & KEY_MOD_CTRL) {
        uint8_t code = key_codes_plain[position];
        return code >= 'a' && code <= 'z' ? code & 0x1f : 0;
    }

    return modifiers & KEY_MOD_SHIFT ? key_codes_shifted[position] : key_codes_plain[position];
}

#ifdef DEVMODE
uint8_t key_decode(uint8_t position, uint8_t modifiers)
{
    return key_code_at(position, modifiers);
}
#endif

// a módosítók mátrixpontjai a key_matrix-ból kikerülnek; a SHIFT LOCK a bal SHIFT pontja
static __forceinline void keyboard_scan(void)
{
    uint8_t select = 0xfe;
    for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
        CIA1_PORT_A = select;
        key_matrix[column] = ~CIA1_PORT_B;
        select = (select << 1) | 1;
    }
    CIA1_PORT_A = 0xff;

    uint8_t modifiers = 0;
    if ((key_matrix[1] & 0x80) || (key_matrix[6] & 0x10)) {  // bal és jobb SHIFT
        modifiers |= KEY_MOD_SHIFT;
    }
    if (key_matrix[7] & 0x24) {                              // CTRL és C=
        modifiers |= KEY_MOD_CTRL;
    }
    key_matrix[1] &= 0x7f;
    key_matrix[6] &= 0xef;
    key_matrix[7] &= 0xdb;
    key_modifiers = modifiers;
}

// képkockánként legfeljebb egy leütés; az egyidejű többi új marad, így a következő képkockákban jön
// a kód nélküli billentyű nem leütés, és nem takarja el a mögötte lévőt
static __forceinline void keypress_detect(void)
{
    uint8_t code = 0;

    for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
        uint8_t down = key_matrix[column];
        uint8_t fresh = down & ~key_counted[column];
        key_counted[column] &= down;

        uint8_t position = column * KEY_COLUMNS, row_bit = 1;
        while (fresh && !code) {
            if (fresh & row_bit) {
                fresh ^= row_bit;
                key_counted[column] |= row_bit;
                code = key_code_at(position, key_modifiers);
            }
            row_bit <<= 1;
            position++;
        }
    }

    if (code) {
        key_press_code = code;
        key_press_count++;
    }
}

// a raszter-IRQ C-ben írt része; az __interrupt menti a fordító zero page regisztereit
__interrupt void irq_body(void)
{
    frame_count++;
    keyboard_scan();
    keypress_detect();
}

// ide mutat a $0314: lefuttatja a C részt, nyugtázza a VIC-et, lépteti a jiffy órát, majd a KERNAL-on át visszatér
__asm irq_stub
{
    jsr irq_body
    lda #$ff
    sta $d019
    jsr $ffea   // UDTIM: jiffy óra
    jsr $ff9f   // SCNKEY, ideiglenes: amíg a key_get a KERNAL pufferéből olvas
    jmp $ea81   // a verem visszaállítása, RTI
}

// ide mutat a $0318: a RESTORE (NMI) nem csinál semmit
__asm nmi_stub
{
    rti
}

static void screen_init(void)
{
    // a felső fél az alsó negáltja
    charset_load(CHARSET_RAM);
    for (uint16_t i = 0; i < 1024; i++) {
        CHARSET_RAM[i + 1024] = ~CHARSET_RAM[i];
    }

    vic_setmode(VICM_TEXT, (const char *)SCREEN_RAM, (const char *)CHARSET_RAM);
}

void screen_color(uint8_t br, uint8_t bg, uint8_t tx)
{
    for (uint8_t i = 0; i < 250; i++) {
        COLOR_RAM[i]       = tx;
        COLOR_RAM[i + 250] = tx;
        COLOR_RAM[i + 500] = tx;
        COLOR_RAM[i + 750] = tx;
    }

    vic.color_border = br;
    vic.color_back   = bg;
}

void screen_clear(void)
{
    for (uint8_t i = 0; i < 250; i++) {
        SCREEN_RAM[i]       = ' ';
        SCREEN_RAM[i + 250] = ' ';
        SCREEN_RAM[i + 500] = ' ';
        SCREEN_RAM[i + 750] = ' ';
    }
}

void screen_put(uint8_t x, uint8_t y, uint8_t ch)
{
    SCREEN_RAM[x + y * SCREEN_W] = ch;
}

void screen_at(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void screen_column(uint8_t x)
{
    cursor_x = x;
}

// az utolsó sorban marad: nem görget
void screen_newline(void)
{
    cursor_x = 0;
    if (cursor_y < SCREEN_H - 1) {
        cursor_y++;
    }
}

// a sor végén levág, nem tör és nem görget
void put_text(const char *text)
{
    while (*text && cursor_x < SCREEN_W) {
        screen_put(cursor_x++, cursor_y, *text++);
    }
}

void put_uint(uint32_t value, int8_t width)
{
    char text[12];
    format_uint(text, value, width);
    put_text(text);
}

void clear_to_line_end(void)
{
    for (uint8_t x = cursor_x; x < SCREEN_W; x++) {
        screen_put(x, cursor_y, ' ');
    }
}

void platform_init(void)
{
    mmap_set(MMAP_NO_BASIC);

    __asm { sei }

    // az induláskor lenyomva tartott billentyű (pl. a RUN RETURN-je) nem leütés
    for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
        key_counted[column] = 0xff;
    }

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
