#include "key.h"

#include <stdbool.h>

#define CIA1_PORT_A  (*(volatile uint8_t *)0xdc00)  // mátrix: oszlopkiválasztás (0 = kiválasztva)
#define CIA1_PORT_B  (*(volatile uint8_t *)0xdc01)  // mátrix: sorolvasás (0 = lenyomva)

volatile uint8_t key_matrix[KEY_COLUMNS];
volatile uint8_t key_modifiers;

static uint8_t key_counted[KEY_COLUMNS];    // a már leütésnek számolt, még lenyomott billentyűk

// körpuffer: az indexek körbefutó számlálók, a különbségük a várakozó leütések száma, így mind a KEY_BUFFER_SIZE hely használható
#define KEY_BUFFER_SIZE  8

// az index a KEY_BUFFER_SIZE - 1 maszkkal fut körbe, és a 8 bites számlálók különbsége nem érheti el a 256-ot
#if (KEY_BUFFER_SIZE & (KEY_BUFFER_SIZE - 1)) != 0 || KEY_BUFFER_SIZE > 128
    #error "a KEY_BUFFER_SIZE 2 hatványa kell legyen, legfeljebb 128"
#endif
static uint8_t key_buffer[KEY_BUFFER_SIZE];
static volatile uint8_t key_buffer_write, key_buffer_read;  // az IRQ csak a write-ot, a játék csak a read-et írja

// a billentyűk kódja mátrixhely szerint (oszlop * KEY_ROWS + sor), oszloponként egy sor; 0 = nincs kódja
static const uint8_t key_codes_plain[KEY_COLUMNS * KEY_ROWS] = {
    KEY_DELETE, KEY_RETURN, 0, 0, 0, 0, 0, 0,          // törlés, RETURN, kurzor, F-billentyűk
    '3', 'w', 'a', '4', 'z', 's', 'e', 0,
    '5', 'r', 'd', '6', 'c', 'f', 't', 'x',
    '7', 'y', 'g', '8', 'b', 'h', 'u', 'v',
    '9', 'i', 'j', '0', 'm', 'k', 'o', 'n',
    '+', 'p', 'l', '-', '.', ':', '@', ',',
    '\\', '*', ';', 0, 0, '=', '^', '/',               // £, *, ;, CLR/HOME, -, =, felfelé nyíl, /
    '1', KEY_ESCAPE, 0, '2', ' ', 0, 'q', KEY_ESCAPE,  // 1, balra nyíl, -, 2, SPACE, -, Q, RUN/STOP
};

static const uint8_t key_codes_shifted[KEY_COLUMNS * KEY_ROWS] = {
    KEY_DELETE, KEY_RETURN, 0, 0, 0, 0, 0, 0,
    '#', 'W', 'A', '$', 'Z', 'S', 'E', 0,
    '%', 'R', 'D', '&', 'C', 'F', 'T', 'X',
    '\'', 'Y', 'G', '(', 'B', 'H', 'U', 'V',
    ')', 'I', 'J', 0, 'M', 'K', 'O', 'N',
    0, 'P', 'L', 0, '>', '[', 0, '<',
    '|', 0, ']', 0, 0, 0, 0, '?',
    '!', KEY_ESCAPE, 0, '"', ' ', 0, 'Q', KEY_ESCAPE,
};

static __forceinline uint8_t key_code_at(uint8_t position, uint8_t modifiers)
{
    if (modifiers & KEY_MODIFIER_CTRL) {
        uint8_t code = key_codes_plain[position];
        return code >= 'a' && code <= 'z' ? code & 0x1f : 0;
    }

    return modifiers & KEY_MODIFIER_SHIFT ? key_codes_shifted[position] : key_codes_plain[position];
}

#ifdef DEVMODE
uint8_t key_decode(uint8_t position, uint8_t modifiers)
{
    return key_code_at(position, modifiers);
}
#endif

// a módosítók mátrixpontjai a key_matrix-ból kikerülnek; a SHIFT LOCK a bal SHIFT pontja
static __forceinline void scan_keyboard(void)
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
        modifiers |= KEY_MODIFIER_SHIFT;
    }
    if (key_matrix[7] & 0x24) {                              // CTRL és C=
        modifiers |= KEY_MODIFIER_CTRL;
    }
    key_matrix[1] &= 0x7f;
    key_matrix[6] &= 0xef;
    key_matrix[7] &= 0xdb;
    key_modifiers = modifiers;
}

// képkockánként legfeljebb egy leütés; az egyidejű többi új marad, így a következő képkockákban jön
// a kód nélküli billentyű nem leütés, és nem takarja el a mögötte lévőt
static __forceinline void detect_keypress(void)
{
    uint8_t code = 0;

    for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
        uint8_t down = key_matrix[column];
        uint8_t fresh = down & ~key_counted[column];
        key_counted[column] &= down;

        uint8_t position = column * KEY_ROWS, row_bit = 1;
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

    // tele pufferbe érkező leütés eldobódik
    if (code && (uint8_t)(key_buffer_write - key_buffer_read) < KEY_BUFFER_SIZE) {
        key_buffer[key_buffer_write & (KEY_BUFFER_SIZE - 1)] = code;
        key_buffer_write++;
    }
}

// a raszter-IRQ része; az __interrupt menti a fordító zero page regisztereit
__interrupt void key_irq(void)
{
    scan_keyboard();
    detect_keypress();
}

void key_init(void)
{
    // az induláskor lenyomva tartott billentyű (pl. a RUN RETURN-je) nem leütés
    for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
        key_counted[column] = 0xff;
    }
}

void wait_for_keys_released(void)
{
    bool any_down;
    do {
        any_down = false;
        for (uint8_t column = 0; column < KEY_COLUMNS; column++) {
            if (key_matrix[column]) {
                any_down = true;
            }
        }
    } while (any_down);
}

uint8_t read_key(void)
{
    if (key_buffer_read == key_buffer_write) {
        return 0;
    }

    uint8_t code = key_buffer[key_buffer_read & (KEY_BUFFER_SIZE - 1)];
    key_buffer_read++;
    return code;
}

uint8_t wait_for_key(void)
{
    uint8_t code;
    do {
        code = read_key();
    } while (!code);

    return code;
}
