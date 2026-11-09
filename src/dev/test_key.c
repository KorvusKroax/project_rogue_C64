#include "tests.h"

#include <stdbool.h>

#include "../platform/key.h"
#include "../platform/key_codes.h"

struct KeyCase {
    uint8_t column, row, modifiers, code;
};

static const struct KeyCase key_cases[] = {
    // SHIFT-től független billentyűk
    { 0, 0, 0, KEY_DELETE }, { 0, 0, KEY_MODIFIER_SHIFT, KEY_DELETE },
    { 0, 1, 0, KEY_RETURN }, { 0, 1, KEY_MODIFIER_SHIFT, KEY_RETURN },
    { 7, 7, 0, KEY_ESCAPE }, { 7, 7, KEY_MODIFIER_SHIFT, KEY_ESCAPE },
    { 7, 1, 0, KEY_ESCAPE }, { 7, 1, KEY_MODIFIER_SHIFT, KEY_ESCAPE },
    { 7, 4, 0, ' ' },        { 7, 4, KEY_MODIFIER_SHIFT, ' ' },
    // írásjelek
    { 5, 4, 0, '.' },  { 5, 4, KEY_MODIFIER_SHIFT, '>' },
    { 5, 7, 0, ',' },  { 5, 7, KEY_MODIFIER_SHIFT, '<' },
    { 6, 7, 0, '/' },  { 6, 7, KEY_MODIFIER_SHIFT, '?' },
    { 5, 5, 0, ':' },  { 5, 5, KEY_MODIFIER_SHIFT, '[' },
    { 6, 2, 0, ';' },  { 6, 2, KEY_MODIFIER_SHIFT, ']' },
    { 6, 0, 0, '\\' }, { 6, 0, KEY_MODIFIER_SHIFT, '|' },
    { 7, 0, 0, '1' },  { 7, 0, KEY_MODIFIER_SHIFT, '!' },
    { 7, 3, 0, '2' },  { 7, 3, KEY_MODIFIER_SHIFT, '"' },
    { 1, 0, 0, '3' },  { 1, 0, KEY_MODIFIER_SHIFT, '#' },
    { 1, 3, 0, '4' },  { 1, 3, KEY_MODIFIER_SHIFT, '$' },
    { 2, 0, 0, '5' },  { 2, 0, KEY_MODIFIER_SHIFT, '%' },
    { 2, 3, 0, '6' },  { 2, 3, KEY_MODIFIER_SHIFT, '&' },
    { 3, 0, 0, '7' },  { 3, 0, KEY_MODIFIER_SHIFT, '\'' },
    { 3, 3, 0, '8' },  { 3, 3, KEY_MODIFIER_SHIFT, '(' },
    { 4, 0, 0, '9' },  { 4, 0, KEY_MODIFIER_SHIFT, ')' },
    { 4, 3, 0, '0' },
    { 6, 6, 0, '^' },  { 5, 6, 0, '@' }, { 6, 1, 0, '*' },
    { 5, 0, 0, '+' },  { 5, 3, 0, '-' }, { 6, 5, 0, '=' },
    // CTRL
    { 3, 5, KEY_MODIFIER_CTRL, 8 },
    { 4, 2, KEY_MODIFIER_CTRL, 10 },
    { 1, 2, KEY_MODIFIER_CTRL | KEY_MODIFIER_SHIFT, 1 },
    { 7, 0, KEY_MODIFIER_CTRL, 0 },
    { 0, 1, KEY_MODIFIER_CTRL, 0 },
    // kód nélkül
    { 0, 2, 0, 0 }, { 0, 3, 0, 0 }, { 6, 3, 0, 0 },
    { 4, 3, KEY_MODIFIER_SHIFT, 0 }, { 5, 6, KEY_MODIFIER_SHIFT, 0 },
};

#define LETTER_COUNT  ('z' - 'a' + 1)

int test_key_decode(uint16_t i)
{
    for (uint8_t c = 0; c < sizeof(key_cases) / sizeof(key_cases[0]); c++) {
        const struct KeyCase *k = &key_cases[c];
        if (get_key_code(k->column * KEY_ROWS + k->row, k->modifiers) != k->code) return false;
    }

    // mind a LETTER_COUNT betű megvan, a SHIFT-es és a CTRL-os változatával
    uint32_t letters_seen = 0;
    for (uint8_t position = 0; position < KEY_COLUMNS * KEY_ROWS; position++) {
        uint8_t code = get_key_code(position, 0);
        if (code >= 'a' && code <= 'z') {
            letters_seen |= 1UL << (code - 'a');
            if (get_key_code(position, KEY_MODIFIER_SHIFT) != code - ('a' - 'A')) return false;
            if (get_key_code(position, KEY_MODIFIER_CTRL) != code - ('a' - 1)) return false;
        }
    }

    return letters_seen == (1UL << LETTER_COUNT) - 1;
}
