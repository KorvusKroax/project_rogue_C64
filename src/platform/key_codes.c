#include "key_codes.h"

#include "key.h"

// a billentyűk kódja mátrixhely szerint (oszlop * KEY_ROWS + sor), oszloponként egy sor; 0 = nincs kódja
static const uint8_t key_codes_plain[KEY_COLUMNS * KEY_ROWS] = {
    KEY_DELETE, KEY_RETURN, 0, KEY_F7, KEY_F1, KEY_F3, KEY_F5, 0,  // törlés, RETURN, kurzor vízszintes, F7, F1, F3, F5, kurzor függőleges
    '3', 'w', 'a', '4', 'z', 's', 'e', 0,
    '5', 'r', 'd', '6', 'c', 'f', 't', 'x',
    '7', 'y', 'g', '8', 'b', 'h', 'u', 'v',
    '9', 'i', 'j', '0', 'm', 'k', 'o', 'n',
    '+', 'p', 'l', '-', '.', ':', '@', ',',
    '\\', '*', ';', 0, 0, '=', '^', '/',               // £, *, ;, CLR/HOME, -, =, felfelé nyíl, /
    '1', KEY_ESCAPE, 0, '2', ' ', 0, 'q', KEY_ESCAPE,  // 1, balra nyíl, -, 2, SPACE, -, Q, RUN/STOP
};

static const uint8_t key_codes_shifted[KEY_COLUMNS * KEY_ROWS] = {
    KEY_DELETE, KEY_RETURN, 0, KEY_F8, KEY_F2, KEY_F4, KEY_F6, 0,
    '#', 'W', 'A', '$', 'Z', 'S', 'E', 0,
    '%', 'R', 'D', '&', 'C', 'F', 'T', 'X',
    '\'', 'Y', 'G', '(', 'B', 'H', 'U', 'V',
    ')', 'I', 'J', 0, 'M', 'K', 'O', 'N',
    0, 'P', 'L', 0, '>', '[', 0, '<',
    '|', 0, ']', 0, 0, 0, 0, '?',
    '!', KEY_ESCAPE, 0, '"', ' ', 0, 'Q', KEY_ESCAPE,
};

uint8_t get_key_code(uint8_t position, uint8_t modifiers)
{
    if (modifiers & KEY_MODIFIER_CTRL) {
        uint8_t code = key_codes_plain[position];
        return code >= 'a' && code <= 'z' ? code & 0x1f : 0;
    }

    return modifiers & KEY_MODIFIER_SHIFT ? key_codes_shifted[position] : key_codes_plain[position];
}
