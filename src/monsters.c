#include "monsters.h"

const struct MonsterType monster_types[MONSTER_TYPE_COUNT] = {
    //      carry  flags                           level armor  exp  segments
    /* A */ {   0, MONSTER_MEAN,                       5,   2,   20, 2, { { 0, 0 }, { 0, 0 } } },
    /* B */ {   0, MONSTER_FLYING,                     1,   3,    1, 1, { { 1, 2 } } },
    /* C */ {  15, 0,                                  4,   4,   17, 3, { { 1, 2 }, { 1, 5 }, { 1, 5 } } },
    /* D */ { 100, MONSTER_MEAN,                      10,  -1, 5000, 3, { { 1, 8 }, { 1, 8 }, { 3, 10 } } },
    /* E */ {   0, MONSTER_MEAN,                       1,   7,    2, 1, { { 1, 2 } } },
    /* F */ {   0, MONSTER_MEAN,                       8,   3,   80, 1, { { DAMAGE_DICE_FLYTRAP, 1 } } },
    /* G */ {  20, MONSTER_MEAN | MONSTER_FLYING,     13,   2, 2000, 2, { { 4, 3 }, { 3, 5 } } },
    /* H */ {   0, MONSTER_MEAN,                       1,   5,    3, 1, { { 1, 8 } } },
    /* I */ {   0, 0,                                  1,   9,    5, 1, { { 0, 0 } } },
    /* J */ {  70, 0,                                 15,   6, 3000, 2, { { 2, 12 }, { 2, 4 } } },
    /* K */ {   0, MONSTER_MEAN | MONSTER_FLYING,      1,   7,    1, 1, { { 1, 4 } } },
    /* L */ {   0, 0,                                  3,   8,   10, 1, { { 1, 1 } } },
    /* M */ {  40, MONSTER_MEAN,                       8,   2,  200, 3, { { 3, 4 }, { 3, 4 }, { 2, 5 } } },
    /* N */ { 100, 0,                                  3,   9,   37, 1, { { 0, 0 } } },
    /* O */ {  15, MONSTER_GREEDY,                     1,   6,    5, 1, { { 1, 8 } } },
    /* P */ {   0, MONSTER_INVISIBLE,                  8,   3,  120, 1, { { 4, 4 } } },
    /* Q */ {   0, MONSTER_MEAN,                       3,   3,   15, 2, { { 1, 5 }, { 1, 5 } } },
    /* R */ {   0, MONSTER_MEAN,                       2,   3,    9, 1, { { 1, 6 } } },
    /* S */ {   0, MONSTER_MEAN,                       1,   5,    2, 1, { { 1, 3 } } },
    /* T */ {  50, MONSTER_MEAN,                       6,   4,  120, 3, { { 1, 8 }, { 1, 8 }, { 2, 6 } } },
    /* U */ {   0, MONSTER_MEAN,                       7,  -2,  190, 3, { { 1, 9 }, { 1, 9 }, { 2, 9 } } },
    /* V */ {  20, MONSTER_MEAN,                       8,   1,  350, 1, { { 1, 10 } } },
    /* W */ {   0, 0,                                  5,   4,   55, 1, { { 1, 6 } } },
    /* X */ {  30, 0,                                  7,   7,  100, 1, { { 4, 4 } } },
    /* Y */ {  30, 0,                                  4,   6,   50, 2, { { 1, 6 }, { 1, 6 } } },
    /* Z */ {   0, MONSTER_MEAN,                       2,   8,    6, 1, { { 1, 8 } } },
};

// a szabad hely type mezője 0, mert a foglalt helyeké mindig 'A'..'Z'
#define FREE_MONSTER_TYPE  0

struct Monster *get_monster(uint8_t index)
{
    return &game.monsters[index - 1];
}

uint8_t allocate_monster(uint8_t type)
{
    for (uint8_t index = 1; index <= MAX_MONSTERS; index++) {
        struct Monster *monster = get_monster(index);
        if (monster->type == FREE_MONSTER_TYPE) {
            monster->type = type;
            return index;
        }
    }
    return NO_MONSTER;
}

void clear_monsters(void)
{
    for (uint8_t index = 1; index <= MAX_MONSTERS; index++) {
        get_monster(index)->type = FREE_MONSTER_TYPE;
    }
}
