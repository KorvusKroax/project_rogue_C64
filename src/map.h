#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>

// a térkép jelei
#define CHAR_PASSAGE           '#'
#define CHAR_DOOR              '+'
#define CHAR_FLOOR             '.'
#define CHAR_PLAYER            '@'
#define CHAR_TRAP              '^'
#define CHAR_STAIRS            '%'
#define CHAR_GOLD              '*'
#define CHAR_POTION            '!'
#define CHAR_SCROLL            '?'
#define CHAR_WALL_HORIZONTAL   '-'
#define CHAR_WALL_VERTICAL     '|'
#define CHAR_FOOD              ':'
#define CHAR_WEAPON            ')'
#define CHAR_ARMOR             ']'
#define CHAR_AMULET            ','
#define CHAR_RING              '='
#define CHAR_STICK             '/'
#define CHAR_MAGIC             '$'
#define CHAR_SOLID_ROCK        ' '

// a mezőflagek; az alsó mező a folyosórendszer sorszáma vagy a csapda típusa, a mező fajtája szerint
#define TILE_PASSAGE         0x80      // folyosó vagy labirintus mező (F_PASS)
#define TILE_SEEN            0x40      // a játékos már látta (F_SEEN)
#define TILE_REAL            0x20      // nem rejtett: ami látszik, az van ott (F_REAL)
#define TILE_MONSTER         0x10      // szörny áll rajta, hogy melyik, azt a szörnylista adja meg (F_MONST)
#define TILE_PASSAGE_NUMBER  0x0f      // folyosó-mezőn a folyosórendszer sorszáma (F_PNUM)
#define TILE_TRAP_TYPE       0x07      // csapda-mezőn a csapda típusa (F_TMASK)

void clear_map(void);                                           // minden mező CHAR_SOLID_ROCK, nem rejtett (TILE_REAL), más flag nélkül

uint8_t get_map_character(uint8_t x, uint8_t y);                // y: MAP_TOP_ROW..MAP_TOP_ROW+MAP_ROWS-1
void set_map_character(uint8_t x, uint8_t y, uint8_t character);

uint8_t get_map_flags(uint8_t x, uint8_t y);                    // a mező összes flagje egy bájtban
void add_map_flags(uint8_t x, uint8_t y, uint8_t flags);        // a megadott flagek bekapcsolása, a többi marad
void remove_map_flags(uint8_t x, uint8_t y, uint8_t flags);     // a megadott flagek kikapcsolása, a többi marad

bool is_walkable_character(uint8_t character);                  // a jel szerint rá lehet lépni: nem kőzet és nem fal; a titkos folyosó kőzetnek, a titkos ajtó falnak látszik

#endif
