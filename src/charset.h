#ifndef CHARSET_H
#define CHARSET_H

#include <stdint.h>

#define GLYPH_FIRST  32
#define GLYPH_LAST   126

// a térkép jelei
#define PASSAGE     '#'
#define DOOR        '+'
#define FLOOR       '.'
#define PLAYER      '@'
#define TRAP        '^'
#define STAIRS      '%'
#define GOLD        '*'
#define POTION      '!'
#define SCROLL      '?'
#define WALL_H      '-'
#define WALL_V      '|'
#define FOOD        ':'
#define WEAPON      ')'
#define ARMOR       ']'
#define AMULET      ','
#define RING        '='
#define STICK       '/'
#define MAGIC       '$'
#define SOLID_ROCK  ' '

// a 0..127. jel képét a target címre írja (csak tiltott megszakítással hívható)
void charset_load(volatile uint8_t *target);

#endif
